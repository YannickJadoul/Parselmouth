/*
 * Copyright (C) 2020-2022  Yannick Jadoul
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

#ifndef PARSELMOUTH_MAKECAPSULE_H
#define PARSELMOUTH_MAKECAPSULE_H

#include <pybind11/pytypes.h>

#include <memory>

template <typename T>
pybind11::capsule make_capsule(std::unique_ptr<T> &&ptr) {
	using RawPtr = typename std::unique_ptr<T>::pointer;
	auto capsule = pybind11::capsule(ptr.get(), [](void *p) { std::unique_ptr<T>(reinterpret_cast<RawPtr>(p)); });
	ptr.release();
	return capsule;
}

#endif //PARSELMOUTH_MAKECAPSULE_H
