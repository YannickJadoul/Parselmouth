/*
 * Copyright (C) 2019  Yannick Jadoul
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

#include "Parselmouth.h"

#include "TextGrid.h"

#include <pybind11/stl.h>

#include <set>
#include <sstream>

namespace py = pybind11;
using namespace py::literals;
using namespace std::string_literals;

using u32ostringstream = std::basic_ostringstream<char32_t>;

namespace parselmouth {

namespace {

py::module importTgt() {
	try {
		return py::module::import("tgt");
	}
	catch (py::error_already_set &e) {
		if (e.matches(PyExc_ImportError))
			throw std::runtime_error("Could not import 'tgt' (TextGridTools).\nMake sure the 'tgt' package is installed, e.g. by running `pip install tgt`");
		else
			throw;
	}
}

py::object toTgtPoint(const py::module &tgt, TextPoint point) {
	return tgt.attr("Point")(point->number, point->mark.get());
}

py::object toTgtPointTier(const py::module &tgt, TextTier tier) {
	auto tgtTier = tgt.attr("PointTier")(tier->xmin, tier->xmax, tier->name.get());
	for (auto i = 1; i <= tier->points.size; ++i)
		tgtTier.attr("add_point")(toTgtPoint(tgt, tier->points.at[i]));
	return tgtTier;
}

py::object toTgtInterval(const py::module &tgt, TextInterval interval) {
	return tgt.attr("Interval")(interval->xmin, interval->xmax, interval->text.get());
}

py::object toTgtIntervalTier(const py::module &tgt, IntervalTier tier) {
	auto tgtTier = tgt.attr("IntervalTier")(tier->xmin, tier->xmax, tier->name.get());
	for (auto i = 1; i <= tier->intervals.size; ++i)
		tgtTier.attr("add_interval")(toTgtInterval(tgt, tier->intervals.at[i]));
	return tgtTier;
}

py::object toTgtTextGrid(TextGrid textGrid) {
	auto tgt = importTgt();

	auto tgtTextGrid = tgt.attr("TextGrid")();
	for (auto i = 1; i <= textGrid->tiers->size; ++i) {
		auto tier = textGrid->tiers->at[i];
		if (tier->classInfo == classTextTier)
			tgtTextGrid.attr("add_tier")(toTgtPointTier(tgt, static_cast<TextTier>(tier)));
		else if (tier->classInfo == classIntervalTier)
			tgtTextGrid.attr("add_tier")(toTgtIntervalTier(tgt, static_cast<IntervalTier>(tier)));
		else
			throw std::runtime_error("Tier type not supported by TextGridTools: "s + Melder_peek32to8(tier->classInfo->className));
	}

	return tgtTextGrid;
}

autoTextPoint fromTgtPoint(const py::handle &tgtPoint) {
	return TextPoint_create(py::cast<double>(tgtPoint.attr("time")), py::cast<std::u32string>(tgtPoint.attr("text")).c_str());
}

autoTextTier fromTgtPointTier(const py::handle &tgtPointTier) {
	auto tier = TextTier_create(py::cast<double>(tgtPointTier.attr("start_time")), py::cast<double>(tgtPointTier.attr("end_time")));
	Thing_setName(tier.get(), py::cast<std::u32string>(tgtPointTier.attr("name")).c_str());
	for (const auto &tgtPoint : tgtPointTier.attr("points")) {
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
	tier->intervals.removeItem(1); // IntervalTier_create adds an empty tier, because IntervalTiers can't be empty
	for (const auto &tgtInterval : tgtIntervalTier.attr("intervals")) {
		tier->intervals.addItem_move(fromTgtInterval(tgtInterval));
	}
	// TODO Check tier not empty
	return tier;
}

autoTextGrid fromTgtTextGrid(const py::object &tgtTextGrid) {
	auto tgt = importTgt();
	auto tgtPointTierType = tgt.attr("PointTier");
	auto tgtIntervalTierType = tgt.attr("IntervalTier");

	auto textGrid = TextGrid_createWithoutTiers(py::cast<double>(tgtTextGrid.attr("start_time")), py::cast<double>(tgtTextGrid.attr("end_time")));
	for (const auto &tgtTier : tgtTextGrid.attr("tiers")) {
		if (py::isinstance(tgtTier, tgtPointTierType)) // TODO Replace by py::isinstance<...>(tgtTier)? Or tier.tier_type()?
			textGrid->tiers->addItem_move(fromTgtPointTier(tgtTier));
		else if (py::isinstance(tgtTier, tgtIntervalTierType))
			textGrid->tiers->addItem_move(fromTgtIntervalTier(tgtTier));
		else
			throw std::runtime_error("Tier type not supported by TextGridTools: "s + py::cast<std::string>(py::str(tgtTier.get_type())));
	}

	return textGrid;
}

} // namespace


PRAAT_CLASS_BINDING(TextGrid) {
	// Note: this overload should come before the `std::vector` overload, since strings can be converted into vectors of characters
	def(py::init([] (double startTime, double endTime, const std::u32string &allTierNames, const std::u32string &pointTierNames) {
		    if (endTime <= startTime) Melder_throw(U"The end time should be greater than the start time");
			    return TextGrid_create(startTime, endTime, allTierNames.c_str(), pointTierNames.c_str());
	    }),
	    "start_time"_a, "end_time"_a, "tier_names"_a, "point_tier_names"_a);

	def(py::init([] (double startTime, double endTime, const std::vector<std::u32string> &allTierNames, const std::vector<std::u32string> &pointTierNames) {
			// TODO Necessary? What about empty TextGrids? No tiers are not allowed by Praat. Also use TextGrid_create (but what about weird names with spaces)?
			// TODO "Name" should be a single ink-word and cannot contain a space.
		    if (endTime <= startTime) Melder_throw(U"The end time should be greater than the start time");

		    std::unordered_set<std::u32string> allTierNamesSet(allTierNames.begin(), allTierNames.end());
		    for (auto &pointTierName : pointTierNames) {
		    	if (!allTierNamesSet.count(pointTierName))
		    		Melder_throw(U"Point tier name '", pointTierName.c_str(), U"' is not in list of all tier names");
		    }

		    std::unordered_set<std::u32string> pointTierNamesSet(pointTierNames.begin(), pointTierNames.end());
		    auto textGrid = TextGrid_createWithoutTiers(startTime, endTime);
		    for (auto &tierName : allTierNames) {
		    	autoFunction tier;
		    	if (pointTierNamesSet.count(tierName))
		    	    tier = TextTier_create(startTime, endTime);
			    else
			    	tier = IntervalTier_create(startTime, endTime);
			    Thing_setName(tier.get(), tierName.c_str());
			    textGrid->tiers->addItem_move(tier.move());
		    }
		    return textGrid;
	    }),
	    "start_time"_a, "end_time"_a, "tier_names"_a = py::list(), "point_tier_names"_a = py::list());

	def("to_tgt",
	    toTgtTextGrid);

	def_static("from_tgt",
	           fromTgtTextGrid,
	           "tgt_text_grid"_a);
}

} // namespace parselmouth
