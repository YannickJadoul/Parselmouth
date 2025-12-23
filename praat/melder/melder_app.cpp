/* melder_app.cpp
 *
 * Copyright (C) 2024,2025 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "melder.h"

static autostring32 theUpperCaseAppName, theLowerCaseAppName;
void Melder_setAppName (conststring32 appName) {
	theUpperCaseAppName = upperCamelCase_STR (appName);
	theLowerCaseAppName = lowerSnakeCase_STR (appName);
}
conststring32 Melder_upperCaseAppName() {
	return theUpperCaseAppName.get();
}
conststring32 Melder_lowerCaseAppName() {
	return theLowerCaseAppName.get();
}

static autostring32 theAppVersionText;
static integer theAppVersionNumber;
void Melder_setAppVersion (conststring32 versionText, integer versionNumber) {
	theAppVersionText = Melder_dup (versionText);
	theAppVersionNumber = versionNumber;
}
conststring32 Melder_appVersionSTR() {
	return theAppVersionText.get();
}
integer Melder_appVersion() {
	return theAppVersionNumber;
}

static integer theAppYear, theAppMonth, theAppDay;
void Melder_setAppDate (integer year, integer month, integer day) {
	theAppYear = year;
	theAppMonth = month;
	theAppDay = day;
}
integer Melder_appYear() {
	return theAppYear;
}
integer Melder_appMonth() {
	return theAppMonth;
}
conststring32 Melder_appMonthSTR() {
	static conststring32 monthNames [12] = {
		U"January", U"February", U"March", U"April", U"May", U"June",
		U"July", U"August", U"September", U"October", U"November", U"December"
	};
	Melder_assert (theAppMonth > 0 && theAppMonth <= 12);
	return monthNames [theAppMonth-1];
}
integer Melder_appDay() {
	return theAppDay;
}

static autostring32 theAppContactAddress;
void Melder_setAppContactAddress (conststring32 firstPartOfEmailAddress, conststring32 secondPartOfEmailAdress) {
	theAppContactAddress = Melder_dup (Melder_cat (firstPartOfEmailAddress, U"@", secondPartOfEmailAdress));
}
conststring32 Melder_appContactAddress() {
	return theAppContactAddress.get();
}

/* End of file melder_app.cpp */
