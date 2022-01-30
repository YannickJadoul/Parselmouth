/*
 * Copyright (C) 2019-2022  Yannick Jadoul
 *
 * This file is part of Parselmouth.
 *
 * Parselmouth is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Parselmouth is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Parselmouth.  If not, see <http://www.gnu.org/licenses/>
 */

#include "TextGridTools.h"

#include <praat/fon/TextGrid.h>

namespace py = pybind11;
using namespace std::string_literals;

namespace parselmouth {
namespace {

py::module_ importTgt() {
	try {
		return py::module_::import("tgt");
	}
	catch (py::error_already_set &e) {
		if (e.matches(PyExc_ImportError))
			throw std::runtime_error(
					"Could not import 'tgt' (TextGridTools).\nMake sure the 'tgt' package is installed, e.g. by running `pip install tgt`");
		else
			throw;
	}
}

py::object toTgtPoint(const py::module_ &tgt, TextPoint point) {
	return tgt.attr("Point")(point->number, point->mark.get());
}

py::object toTgtPointTier(const py::module_ &tgt, TextTier tier) {
	auto tgtTier = tgt.attr("PointTier")(tier->xmin, tier->xmax, tier->name.get());
	for (auto i = 1; i <= tier->points.size; ++i)
		tgtTier.attr("add_point")(toTgtPoint(tgt, tier->points.at[i]));
	return tgtTier;
}

py::object toTgtInterval(const py::module_ &tgt, TextInterval interval) {
	return tgt.attr("Interval")(interval->xmin, interval->xmax, interval->text.get());
}

py::object toTgtIntervalTier(const py::module_ &tgt, IntervalTier tier, bool includeEmptyIntervals) {
	auto tgtTier = tgt.attr("IntervalTier")(tier->xmin, tier->xmax, tier->name.get());
	for (auto i = 1; i <= tier->intervals.size; ++i) {
		auto interval = tier->intervals.at[i];
		if (includeEmptyIntervals || (interval->text.get() && interval->text[0] != U'\0'))
			tgtTier.attr("add_interval")(toTgtInterval(tgt, interval));
	}
	return tgtTier;
}

autoTextPoint fromTgtPoint(const py::handle &tgtPoint) {
	return TextPoint_create(py::cast<double>(tgtPoint.attr("time")), py::cast<std::u32string>(tgtPoint.attr("text")).c_str());
}

autoTextTier fromTgtPointTier(const py::handle &tgtPointTier) {
	auto tier = TextTier_create(py::cast<double>(tgtPointTier.attr("start_time")), py::cast<double>(tgtPointTier.attr("end_time")));
	Thing_setName(tier.get(), py::cast<std::u32string>(tgtPointTier.attr("name")).c_str());
	for (auto tgtPoint : tgtPointTier.attr("points")) {
		tier->points.addItem_move(fromTgtPoint(tgtPoint));
	}
	return tier;
}

autoTextInterval fromTgtInterval(const py::handle &tgtInterval) {
	return TextInterval_create(py::cast<double>(tgtInterval.attr("start_time")), py::cast<double>(tgtInterval.attr("end_time")), py::cast<std::u32string>(tgtInterval.attr("text")).c_str());
}


autoIntervalTier fromTgtIntervalTier(const py::handle &tgtIntervalTier) {
	auto tier = IntervalTier_create(py::cast<double>(tgtIntervalTier.attr("start_time")), py::cast<double>(tgtIntervalTier.attr("end_time")));
	Thing_setName(tier.get(), py::cast<std::u32string>(tgtIntervalTier.attr("name")).c_str());
	if (py::len(tgtIntervalTier.attr("intervals")) > 0) {
		tier->intervals.removeItem(1); // IntervalTier_create adds an empty tier, because IntervalTiers can't be empty
		for (const auto &tgtInterval : tgtIntervalTier.attr("intervals")) {
			tier->intervals.addItem_move(fromTgtInterval(tgtInterval));
		}
	}
	return tier;
}

} // namespace

// TODO More elaborate `includeEmptyIntervals`, like tgt (True, string, or list of strings)
TgtTextGrid toTgtTextGrid(TextGrid textGrid, bool includeEmptyIntervals /* = false */) {
	auto tgt = importTgt();

	auto tgtTextGrid = tgt.attr("TextGrid")();
	for (auto i = 1; i <= textGrid->tiers->size; ++i) {
		auto tier = textGrid->tiers->at[i];
		if (tier->classInfo == classTextTier)
			tgtTextGrid.attr("add_tier")(toTgtPointTier(tgt, static_cast<TextTier>(tier)));
		else if (tier->classInfo == classIntervalTier)
			tgtTextGrid.attr("add_tier")(toTgtIntervalTier(tgt, static_cast<IntervalTier>(tier), includeEmptyIntervals));
		else
			throw std::runtime_error("Tier type not supported by TextGridTools: "s + Melder_peek32to8(tier->classInfo->className));
	}

	return tgtTextGrid;
}

autoTextGrid fromTgtTextGrid(TgtTextGrid tgtTextGrid) {
	auto tgt = importTgt();
	auto tgtPointTierType = tgt.attr("PointTier");
	auto tgtIntervalTierType = tgt.attr("IntervalTier");

	tgtTextGrid = tgt.attr("io").attr("correct_start_end_times_and_fill_gaps")(tgtTextGrid);

	auto textGrid = TextGrid_createWithoutTiers(py::cast<double>(tgtTextGrid.attr("start_time")), py::cast<double>(tgtTextGrid.attr("end_time")));
	for (auto tgtTier : tgtTextGrid.attr("tiers")) {
		if (py::isinstance(tgtTier, tgtPointTierType))
			textGrid->tiers->addItem_move(fromTgtPointTier(tgtTier));
		else if (py::isinstance(tgtTier, tgtIntervalTierType))
			textGrid->tiers->addItem_move(fromTgtIntervalTier(tgtTier));
		else
			throw std::runtime_error("Tier type not supported by TextGridTools: "s + py::cast<std::string>(py::str(py::type::of(tgtTier))));
	}

	return textGrid;
}

} // namespace parselmouth
