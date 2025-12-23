/* NUMrandom.cpp
 *
 * Copyright (C) 1992-2006,2008,2011,2012,2014-2018,2020,2025 Paul Boersma
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

/* 
   A C-program for MT19937-64 (2014/2/23 version).
   Coded by Takuji Nishimura and Makoto Matsumoto.

   This is a 64-bit version of Mersenne Twister pseudorandom number
   generator.

   Before using, initialize the state by using init_genrand64(seed)  
   or init_by_array64(init_key, key_length).

   Copyright (C) 2004, 2014, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.                          

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote 
        products derived from this software without specific prior written 
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

   References:
   T. Nishimura, ``Tables of 64-bit Mersenne Twisters''
     ACM Transactions on Modeling and 
     Computer Simulation 10. (2000) 348--357.
   M. Matsumoto and T. Nishimura,
     ``Mersenne Twister: a 623-dimensionally equidistributed
       uniform pseudorandom number generator''
     ACM Transactions on Modeling and 
     Computer Simulation 8. (Jan. 1998) 3--30.

   Any feedback is very welcome.
   http://www.math.hiroshima-u.ac.jp/~m-mat/MT/emt.html
   email: m-mat @ math.sci.hiroshima-u.ac.jp (remove spaces)
*/

#if defined (__MINGW32__) || defined (linux)
	#define UINT64_C(n)  n ## ULL
#endif
#include <unistd.h>
#include "melder.h"
#include <chrono>

#define NN  312
#define MM  156
#define MATRIX_A  UINT64_C (0xB5026F5AA96619E9)
#define UM  UINT64_C (0xFFFFFFFF80000000) /* Most significant 33 bits */
#define LM  UINT64_C (0x7FFFFFFF) /* Least significant 31 bits */

class NUMrandom_State { public:

	/** The state vector.
	 */
	uint64 array [NN];

	/** The pointer into the state vector.
		Equals NN + 1 iff the array has not been initialized.
	 */
	int index;
	NUMrandom_State () : index (NN + 1) {}
		// this initialization will lead to an immediate assertion violation
		// when NUMrandomFraction() is called without NUMrandom_initXXX() having been called before;
		// without this initialization, it would be detected only after 312 calls to NUMrandomFraction()

	bool secondAvailable;
	double y;

	/**
		Initialize the whole array with one seed.
		This can be used for testing whether our implementation is correct (i.e. predicts the correct published sequence)
		and perhaps for generating reproducible sequences.
	 */
	uint64 init_genrand64 (uint64 seed) {
		array [0] = seed;
		for (index = 1; index < NN; index ++) {
			array [index] =
				(UINT64_C (6364136223846793005) * (array [index - 1] ^ (array [index - 1] >> 62))
				+ (uint64) index);
		}
		secondAvailable = false;   // ensure consistent starting state even after calling NUMrandomGauss an odd number of times
		return array [NN - 1];
	}

	/* initialize by an array with array-length */
	/* init_key is the array for initializing keys */
	/* key_length is its length */
	void init_by_array64 (uint64 init_key[], unsigned int key_length);

} states [NUMrandom_numberOfChannels];

/* initialize the array with a number of seeds */
void NUMrandom_State :: init_by_array64 (uint64 init_key [], unsigned int key_length)
{
	init_genrand64 (UINT64_C (19650218));   // warm it up

	unsigned int i = 1, j = 0;
	unsigned int k = ( NN > key_length ? NN : key_length );
	for (; k; k --) {
		array [i] = (array [i] ^ ((array [i - 1] ^ (array [i - 1] >> 62)) * UINT64_C (3935559000370003845)))
				+ init_key [j] + (uint64) j;   // non-linear
		i ++;
		j ++;
		if (i >= NN) { array [0] = array [NN - 1]; i = 1; }
		if (j >= key_length) j = 0;
	}
	for (k = NN - 1; k; k --) {
		array [i] = (array [i] ^ ((array [i - 1] ^ (array [i - 1] >> 62)) * UINT64_C (2862933555777941757)))
				- (uint64) i;   // non-linear
		i ++;
		if (i >= NN) { array [0] = array [NN - 1]; i = 1; }
	}

	array [0] = UINT64_C (1) << 63;   // MSB is 1; assuring non-zero initial array
}

static uint64 getTicksSince1969 () {
	using namespace std::chrono;
	const auto timePoint = system_clock::now ();
	const auto duration = timePoint. time_since_epoch ();
	return uint64 (duration. count ());
	//Melder_casual (U"ticks since 1969: ", ticksSince1969);
}

static uint64 getTicksSinceBoot () {
	using namespace std::chrono;
	const auto timePoint = high_resolution_clock::now ();
	const auto duration = timePoint. time_since_epoch ();
	return uint64 (duration. count ());
	//Melder_casual (U"ticks since boot: ", ticksSinceBoot);
}

static bool theInited = false;
void NUMrandom_initializeSafelyAndUnpredictably () {
	const double startingTime = Melder_clock();
	const uint64 ticksSince1969 = getTicksSince1969 ();   // possibly microseconds
	const uint64 ticksSinceBoot = getTicksSinceBoot ();   // possibly nanoseconds
	for (integer ichan = 0; ichan < NUMrandom_numberOfChannels; ichan ++) {
		constexpr integer numberOfKeys = 7;
		uint64 keys [numberOfKeys];
		keys [0] = ticksSince1969;   // unique between boots of the same computer
		keys [1] = UINT64_C (7320321686725470078) + uint64 (ichan);   // unique between threads in the same process
		switch (ichan) {
			/*
				From case 43 on, these numbers were generated by bash:
				{
					od -An -N5760 -t u8 /dev/urandom | tr -s ' ' '\n' | grep -v '^$' > random720.txt
				}
				and by Praat:
				{
					numbers$# = readLinesFromFile$#: "random720.txt"
					writeInfoLine: "Generating..."
					for line to 360
						appendInfoLine: tab$, tab$, tab$, "case ", padLeft$ (string$ (line + 42), 3),
						... ": keys [2] = UINT64_C (", padLeft$ (numbers$# [2*line-1], 20),
						... "); keys [3] = UINT64_C (", padLeft$ (numbers$# [2*line], 20), "); break;"
					endfor
				}
			*/
			case   0: keys [2] = UINT64_C ( 4492812493098689432); keys [3] = UINT64_C ( 8902321878452586268); break;
			case   1: keys [2] = UINT64_C ( 1875086582568685862); keys [3] = UINT64_C (12243257483652989599); break;
			case   2: keys [2] = UINT64_C ( 9040925727554857487); keys [3] = UINT64_C ( 8037578605604605534); break;
			case   3: keys [2] = UINT64_C (11168476768576857685); keys [3] = UINT64_C ( 7862359785763816517); break;
			case   4: keys [2] = UINT64_C ( 3878901748368466876); keys [3] = UINT64_C ( 3563078257726526076); break;
			case   5: keys [2] = UINT64_C ( 2185735817578415800); keys [3] = UINT64_C (  198502654671560756); break;
			case   6: keys [2] = UINT64_C (12248047509814562486); keys [3] = UINT64_C ( 9836250167165762757); break;
			case   7: keys [2] = UINT64_C (   28362088588870143); keys [3] = UINT64_C ( 8756376201767075602); break;
			case   8: keys [2] = UINT64_C ( 5758130586486546775); keys [3] = UINT64_C ( 4213784157469743413); break;
			case   9: keys [2] = UINT64_C ( 8508416536565170756); keys [3] = UINT64_C ( 2856175717654375656); break;
			case  10: keys [2] = UINT64_C ( 2802356275260644756); keys [3] = UINT64_C ( 2309872134087235167); break;
			case  11: keys [2] = UINT64_C (  230875784065064545); keys [3] = UINT64_C ( 1209802371478023476); break;
			case  12: keys [2] = UINT64_C ( 6520185868568714577); keys [3] = UINT64_C ( 2173615001556504015); break;
			case  13: keys [2] = UINT64_C ( 9082605608605765650); keys [3] = UINT64_C ( 1204167447560475647); break;
			case  14: keys [2] = UINT64_C ( 1238716515545475765); keys [3] = UINT64_C ( 8435674023875847388); break;
			case  15: keys [2] = UINT64_C ( 6127715675014756456); keys [3] = UINT64_C ( 2435788450287508457); break;
			case  16: keys [2] = UINT64_C ( 1081237546238975884); keys [3] = UINT64_C ( 2939783238574293882); break;
			case  17: keys [2] = UINT64_C ( 8375036306567876574); keys [3] = UINT64_C ( 8540574357184754857); break;
			case  18: keys [2] = UINT64_C ( 3850245655647557647); keys [3] = UINT64_C ( 4364576692676547674); break;
			case  19: keys [2] = UINT64_C ( 2057843567436577565); keys [3] = UINT64_C ( 8438578345787893745); break;
			case  20: keys [2] = UINT64_C (10123890498732874894); keys [3] = UINT64_C (12091287549854854899); break;
			case  21: keys [2] = UINT64_C ( 2318912478924387389); keys [3] = UINT64_C ( 1209397927247838139); break;
			case  22: keys [2] = UINT64_C (  875843754798401029); keys [3] = UINT64_C (12030941286537547893); break;
			case  23: keys [2] = UINT64_C (10139892874834379012); keys [3] = UINT64_C ( 9129821378428732498); break;
			case  24: keys [2] = UINT64_C (  919818490388575879); keys [3] = UINT64_C (10939848610941094810); break;
			case  25: keys [2] = UINT64_C ( 9129839465819017879); keys [3] = UINT64_C ( 9189102791272104190); break;
			case  26: keys [2] = UINT64_C ( 4756874192190000982); keys [3] = UINT64_C ( 3109570210921901080); break;
			case  27: keys [2] = UINT64_C (12183112903019492499); keys [3] = UINT64_C (12097659104981290001); break;
			case  28: keys [2] = UINT64_C ( 9028903478201080929); keys [3] = UINT64_C ( 4834934012959700481); break;
			case  29: keys [2] = UINT64_C ( 2194569100499384010); keys [3] = UINT64_C ( 1839749894208298429); break;
			case  30: keys [2] = UINT64_C ( 3983209023438537234); keys [3] = UINT64_C ( 7861159102090250989); break;
			case  31: keys [2] = UINT64_C ( 2194849810939034937); keys [3] = UINT64_C ( 3119014928483094870); break;
			case  32: keys [2] = UINT64_C ( 3048135691040185393); keys [3] = UINT64_C (  192186502167157518); break;
			case  33: keys [2] = UINT64_C (12190902471094839890); keys [3] = UINT64_C ( 8737946092598409589); break;
			case  34: keys [2] = UINT64_C ( 3599358408609356095); keys [3] = UINT64_C ( 8937295898493095840); break;
			case  35: keys [2] = UINT64_C (   45076835923445695); keys [3] = UINT64_C (  194634597229574509); break;
			case  36: keys [2] = UINT64_C (  836585748558470458); keys [3] = UINT64_C ( 2289506565038756578); break;
			case  37: keys [2] = UINT64_C (  832563659023943438); keys [3] = UINT64_C ( 1238264921469198284); break;
			case  38: keys [2] = UINT64_C (10393949889702093820); keys [3] = UINT64_C ( 2198245910013109200); break;
			case  39: keys [2] = UINT64_C ( 2001909384819394839); keys [3] = UINT64_C (13716710394194359394); break;
			case  40: keys [2] = UINT64_C (14966767595485485049); keys [3] = UINT64_C ( 1889568829023490839); break;
			case  41: keys [2] = UINT64_C ( 3856928438491014839); keys [3] = UINT64_C ( 4411281012393465812); break;
			case  42: keys [2] = UINT64_C ( 1093814878568456475); keys [3] = UINT64_C ( 1938904975709295894); break;
			case  43: keys [2] = UINT64_C ( 1999120999673999449); keys [3] = UINT64_C ( 1073742433098461328); break;
			case  44: keys [2] = UINT64_C (15463370745733033042); keys [3] = UINT64_C ( 3330984135269988686); break;
			case  45: keys [2] = UINT64_C (  226546627166092375); keys [3] = UINT64_C ( 5328809566422720578); break;
			case  46: keys [2] = UINT64_C ( 9766585977461657535); keys [3] = UINT64_C ( 7596195266339884116); break;
			case  47: keys [2] = UINT64_C ( 3413237725250192239); keys [3] = UINT64_C ( 3499743413934520267); break;
			case  48: keys [2] = UINT64_C ( 2197074319883059433); keys [3] = UINT64_C (  885142809295300412); break;
			case  49: keys [2] = UINT64_C (15265121598896083743); keys [3] = UINT64_C (17905995093362449179); break;
			case  50: keys [2] = UINT64_C (14589148594904176028); keys [3] = UINT64_C ( 5549565292844039665); break;
			case  51: keys [2] = UINT64_C ( 7558544799093516364); keys [3] = UINT64_C (10856573070200366196); break;
			case  52: keys [2] = UINT64_C ( 9394153025520156123); keys [3] = UINT64_C (12269117282462796633); break;
			case  53: keys [2] = UINT64_C ( 4285715285908525420); keys [3] = UINT64_C ( 7140906673407626087); break;
			case  54: keys [2] = UINT64_C (14958579046457267589); keys [3] = UINT64_C (18040195529061384091); break;
			case  55: keys [2] = UINT64_C ( 4392625004927936762); keys [3] = UINT64_C ( 5117914206787554020); break;
			case  56: keys [2] = UINT64_C ( 4744091851176637746); keys [3] = UINT64_C (  408654708196451940); break;
			case  57: keys [2] = UINT64_C (14768115352250233751); keys [3] = UINT64_C ( 8729969118949776553); break;
			case  58: keys [2] = UINT64_C (  605620446538282985); keys [3] = UINT64_C (17880915155958574586); break;
			case  59: keys [2] = UINT64_C (11025548410855584918); keys [3] = UINT64_C (12110318544866173630); break;
			case  60: keys [2] = UINT64_C ( 8419054120584636970); keys [3] = UINT64_C (10197594395520015729); break;
			case  61: keys [2] = UINT64_C ( 4847615393173144686); keys [3] = UINT64_C ( 6618411949653572503); break;
			case  62: keys [2] = UINT64_C (11862519696363521511); keys [3] = UINT64_C ( 6787402781005628523); break;
			case  63: keys [2] = UINT64_C (17158385136024712740); keys [3] = UINT64_C (10204022906670713765); break;
			case  64: keys [2] = UINT64_C ( 8830089593146385705); keys [3] = UINT64_C (16338103625509272841); break;
			case  65: keys [2] = UINT64_C ( 3398767798222355085); keys [3] = UINT64_C (16292972130870278205); break;
			case  66: keys [2] = UINT64_C (14903648774213285028); keys [3] = UINT64_C ( 5026825809267966349); break;
			case  67: keys [2] = UINT64_C (13165869744205861203); keys [3] = UINT64_C (17704215103505493477); break;
			case  68: keys [2] = UINT64_C (  968195875225724167); keys [3] = UINT64_C (10106117107066095211); break;
			case  69: keys [2] = UINT64_C (14588817048164514967); keys [3] = UINT64_C ( 8610093676739640454); break;
			case  70: keys [2] = UINT64_C (11657541396078515547); keys [3] = UINT64_C (15637486936456767082); break;
			case  71: keys [2] = UINT64_C (11514999712457665711); keys [3] = UINT64_C ( 9617063164367659726); break;
			case  72: keys [2] = UINT64_C (  219355128280620311); keys [3] = UINT64_C (13749308772874520332); break;
			case  73: keys [2] = UINT64_C (13966831310273121717); keys [3] = UINT64_C (  937018068065126572); break;
			case  74: keys [2] = UINT64_C ( 6284891740089043951); keys [3] = UINT64_C ( 6771587709442454387); break;
			case  75: keys [2] = UINT64_C ( 7706024653104438384); keys [3] = UINT64_C ( 4748067289819084829); break;
			case  76: keys [2] = UINT64_C (15638651530347444511); keys [3] = UINT64_C ( 8975435322817811255); break;
			case  77: keys [2] = UINT64_C (17682474546230703111); keys [3] = UINT64_C (  918810876351873930); break;
			case  78: keys [2] = UINT64_C ( 1479508298132016404); keys [3] = UINT64_C (11557750287376725524); break;
			case  79: keys [2] = UINT64_C (11930426490491492012); keys [3] = UINT64_C (12806812680450105027); break;
			case  80: keys [2] = UINT64_C (13627485278528308625); keys [3] = UINT64_C (11453098363022675233); break;
			case  81: keys [2] = UINT64_C (11282620072988590391); keys [3] = UINT64_C (  119298038150201436); break;
			case  82: keys [2] = UINT64_C ( 1695575895990520908); keys [3] = UINT64_C ( 7616526814263350254); break;
			case  83: keys [2] = UINT64_C (17857108781380941931); keys [3] = UINT64_C ( 6643452908584742125); break;
			case  84: keys [2] = UINT64_C (13525459596485855938); keys [3] = UINT64_C ( 7917215897609847441); break;
			case  85: keys [2] = UINT64_C (13062944405842513108); keys [3] = UINT64_C (12559150685185202511); break;
			case  86: keys [2] = UINT64_C (16829754319273224605); keys [3] = UINT64_C (14199873585006282404); break;
			case  87: keys [2] = UINT64_C (10270073010275486667); keys [3] = UINT64_C ( 5240002068800782251); break;
			case  88: keys [2] = UINT64_C ( 2694092062206350253); keys [3] = UINT64_C ( 3436373516605717388); break;
			case  89: keys [2] = UINT64_C (10174974910023318803); keys [3] = UINT64_C (15593594378175995408); break;
			case  90: keys [2] = UINT64_C ( 5232131431669557349); keys [3] = UINT64_C ( 5138437074461125632); break;
			case  91: keys [2] = UINT64_C ( 6880809153001193559); keys [3] = UINT64_C ( 5850162627128367895); break;
			case  92: keys [2] = UINT64_C ( 9687293831278134006); keys [3] = UINT64_C ( 8866259361137465086); break;
			case  93: keys [2] = UINT64_C (14327185302986565937); keys [3] = UINT64_C (11083019262706650660); break;
			case  94: keys [2] = UINT64_C ( 1967358594069565059); keys [3] = UINT64_C ( 7736169331994147594); break;
			case  95: keys [2] = UINT64_C ( 3629649210752306625); keys [3] = UINT64_C (18107967879842501556); break;
			case  96: keys [2] = UINT64_C ( 6347484257468125577); keys [3] = UINT64_C (12709783765688215669); break;
			case  97: keys [2] = UINT64_C ( 9117081181834688003); keys [3] = UINT64_C (15808165551579734342); break;
			case  98: keys [2] = UINT64_C ( 3789747092300871382); keys [3] = UINT64_C ( 6232671376346921071); break;
			case  99: keys [2] = UINT64_C (18107826712329726531); keys [3] = UINT64_C (10033332451782418770); break;
			case 100: keys [2] = UINT64_C (11139752540348219824); keys [3] = UINT64_C ( 2724440399604508951); break;
			case 101: keys [2] = UINT64_C ( 3445156385955816372); keys [3] = UINT64_C (16884077675276209151); break;
			case 102: keys [2] = UINT64_C (  410154042799768091); keys [3] = UINT64_C ( 5899426036522089375); break;
			case 103: keys [2] = UINT64_C (10587249363904486585); keys [3] = UINT64_C (12352288102241297736); break;
			case 104: keys [2] = UINT64_C (14326646307690932099); keys [3] = UINT64_C (14003105561909481099); break;
			case 105: keys [2] = UINT64_C (10470528487119371950); keys [3] = UINT64_C (10687875309528030113); break;
			case 106: keys [2] = UINT64_C ( 5706060093599710743); keys [3] = UINT64_C ( 9219672467315708728); break;
			case 107: keys [2] = UINT64_C (10990225828118705180); keys [3] = UINT64_C (10376208065685538769); break;
			case 108: keys [2] = UINT64_C ( 8828841433511521037); keys [3] = UINT64_C ( 3636703634444521537); break;
			case 109: keys [2] = UINT64_C ( 4049344993998357405); keys [3] = UINT64_C ( 5561989679160638557); break;
			case 110: keys [2] = UINT64_C ( 9592596081046894513); keys [3] = UINT64_C (15140170293931025325); break;
			case 111: keys [2] = UINT64_C ( 3446930747671987549); keys [3] = UINT64_C ( 2033337012988975056); break;
			case 112: keys [2] = UINT64_C ( 2480092515815371683); keys [3] = UINT64_C ( 6315574258669719076); break;
			case 113: keys [2] = UINT64_C ( 5720125197160899421); keys [3] = UINT64_C (15495383946365000043); break;
			case 114: keys [2] = UINT64_C ( 3212556350541585065); keys [3] = UINT64_C (17708864754068537729); break;
			case 115: keys [2] = UINT64_C (12646099308128290656); keys [3] = UINT64_C (10120893952339866250); break;
			case 116: keys [2] = UINT64_C ( 8101685383144330269); keys [3] = UINT64_C (17805791550247379430); break;
			case 117: keys [2] = UINT64_C ( 4087206292166357077); keys [3] = UINT64_C ( 6784922391446647119); break;
			case 118: keys [2] = UINT64_C ( 2915264474690618991); keys [3] = UINT64_C (15093649705836182267); break;
			case 119: keys [2] = UINT64_C (10403680620702252071); keys [3] = UINT64_C (  620621478352985866); break;
			case 120: keys [2] = UINT64_C ( 1636863524363622568); keys [3] = UINT64_C (11705471825327804675); break;
			case 121: keys [2] = UINT64_C (  440804476171611678); keys [3] = UINT64_C (16246525235969107217); break;
			case 122: keys [2] = UINT64_C (14860013397744154363); keys [3] = UINT64_C ( 4471408017294611102); break;
			case 123: keys [2] = UINT64_C ( 5003471241276682502); keys [3] = UINT64_C ( 7207510683236129615); break;
			case 124: keys [2] = UINT64_C (15088678146245296461); keys [3] = UINT64_C (  184498597431333320); break;
			case 125: keys [2] = UINT64_C (13827097183637072864); keys [3] = UINT64_C (16896306893294956176); break;
			case 126: keys [2] = UINT64_C ( 6996230867532778483); keys [3] = UINT64_C (17682222973540793359); break;
			case 127: keys [2] = UINT64_C ( 7638909400307634833); keys [3] = UINT64_C ( 7317360191993228846); break;
			case 128: keys [2] = UINT64_C (17111123235929112583); keys [3] = UINT64_C (15648808815783630324); break;
			case 129: keys [2] = UINT64_C ( 3447794153774737815); keys [3] = UINT64_C ( 5555872596116290411); break;
			case 130: keys [2] = UINT64_C ( 3356574978607179548); keys [3] = UINT64_C ( 3564500428400262837); break;
			case 131: keys [2] = UINT64_C ( 4635092689879018409); keys [3] = UINT64_C ( 5741770286738188015); break;
			case 132: keys [2] = UINT64_C ( 9512633354420261422); keys [3] = UINT64_C (12095866101441244815); break;
			case 133: keys [2] = UINT64_C (17498679347179794894); keys [3] = UINT64_C ( 1595417421901097293); break;
			case 134: keys [2] = UINT64_C (15836532543945201133); keys [3] = UINT64_C (  330491073955258611); break;
			case 135: keys [2] = UINT64_C (  140610662857217573); keys [3] = UINT64_C ( 9215953464155066310); break;
			case 136: keys [2] = UINT64_C ( 7715374817234726298); keys [3] = UINT64_C (15021912597961891402); break;
			case 137: keys [2] = UINT64_C (12675059238977069966); keys [3] = UINT64_C (13819555790783911546); break;
			case 138: keys [2] = UINT64_C ( 1769685980903887122); keys [3] = UINT64_C (14118768808335083068); break;
			case 139: keys [2] = UINT64_C ( 9641420094482971293); keys [3] = UINT64_C (10210320065289824970); break;
			case 140: keys [2] = UINT64_C (11694123593138116701); keys [3] = UINT64_C ( 9568512583755435572); break;
			case 141: keys [2] = UINT64_C (12565267581237054474); keys [3] = UINT64_C (10723931576375379176); break;
			case 142: keys [2] = UINT64_C ( 6543741011939339939); keys [3] = UINT64_C (17527125543000130902); break;
			case 143: keys [2] = UINT64_C (12558149843106116110); keys [3] = UINT64_C (  471994347818964341); break;
			case 144: keys [2] = UINT64_C ( 9567815523491491329); keys [3] = UINT64_C ( 5764392353437396795); break;
			case 145: keys [2] = UINT64_C ( 4732066962405775498); keys [3] = UINT64_C (10154673526876135738); break;
			case 146: keys [2] = UINT64_C ( 2881819828044806447); keys [3] = UINT64_C ( 5675637098496097366); break;
			case 147: keys [2] = UINT64_C (16498815124898668838); keys [3] = UINT64_C ( 4920274030354320751); break;
			case 148: keys [2] = UINT64_C ( 2884395162934050326); keys [3] = UINT64_C (15474479435196770618); break;
			case 149: keys [2] = UINT64_C ( 6158086123481539746); keys [3] = UINT64_C (17420930438741019656); break;
			case 150: keys [2] = UINT64_C (10857999337087018653); keys [3] = UINT64_C (16115565595008606095); break;
			case 151: keys [2] = UINT64_C (11948313743440269123); keys [3] = UINT64_C (17283269037224019981); break;
			case 152: keys [2] = UINT64_C ( 7282383136907335856); keys [3] = UINT64_C ( 8475008059763441032); break;
			case 153: keys [2] = UINT64_C (17424773590971838843); keys [3] = UINT64_C ( 7013721679356471137); break;
			case 154: keys [2] = UINT64_C ( 7301767788970450313); keys [3] = UINT64_C (10707886374421671563); break;
			case 155: keys [2] = UINT64_C (12826555591611291505); keys [3] = UINT64_C ( 5627311230378439490); break;
			case 156: keys [2] = UINT64_C (12114491187148894508); keys [3] = UINT64_C (10991859044082790587); break;
			case 157: keys [2] = UINT64_C (  183629502502310360); keys [3] = UINT64_C (14373315261239258468); break;
			case 158: keys [2] = UINT64_C ( 9135132813948428862); keys [3] = UINT64_C ( 3004002693330407276); break;
			case 159: keys [2] = UINT64_C ( 4682804553675128920); keys [3] = UINT64_C ( 7221841666440149128); break;
			case 160: keys [2] = UINT64_C (15948822416707198783); keys [3] = UINT64_C ( 2976345958281965403); break;
			case 161: keys [2] = UINT64_C (15169991984060256411); keys [3] = UINT64_C (10747211365934459927); break;
			case 162: keys [2] = UINT64_C ( 9668059066614010628); keys [3] = UINT64_C (16994143092536402971); break;
			case 163: keys [2] = UINT64_C (14370180978005261814); keys [3] = UINT64_C (  706570114896305456); break;
			case 164: keys [2] = UINT64_C ( 5740369527765296403); keys [3] = UINT64_C (16590842298233132384); break;
			case 165: keys [2] = UINT64_C ( 6513348852946879559); keys [3] = UINT64_C ( 7633926316087486071); break;
			case 166: keys [2] = UINT64_C ( 3059492495243560265); keys [3] = UINT64_C ( 9907897502012597054); break;
			case 167: keys [2] = UINT64_C (14241149687139951388); keys [3] = UINT64_C ( 9611124970660472808); break;
			case 168: keys [2] = UINT64_C ( 5814575064842185690); keys [3] = UINT64_C (16446598900815922485); break;
			case 169: keys [2] = UINT64_C (17048146351351202879); keys [3] = UINT64_C ( 2765175908148465375); break;
			case 170: keys [2] = UINT64_C ( 8941896722656320099); keys [3] = UINT64_C ( 6777842417242615669); break;
			case 171: keys [2] = UINT64_C (18403428637386623351); keys [3] = UINT64_C ( 4403243688487584802); break;
			case 172: keys [2] = UINT64_C (14589568185038795199); keys [3] = UINT64_C ( 7974411057984039063); break;
			case 173: keys [2] = UINT64_C ( 9395008199707855535); keys [3] = UINT64_C ( 6768753279604860706); break;
			case 174: keys [2] = UINT64_C ( 7042203057549851040); keys [3] = UINT64_C (13110771231523999817); break;
			case 175: keys [2] = UINT64_C ( 1346764711708877024); keys [3] = UINT64_C (16779861934624828441); break;
			case 176: keys [2] = UINT64_C ( 5833419699656525000); keys [3] = UINT64_C ( 8267751534515073700); break;
			case 177: keys [2] = UINT64_C (12904099657557304924); keys [3] = UINT64_C ( 4690948926827849038); break;
			case 178: keys [2] = UINT64_C ( 3468012342537987822); keys [3] = UINT64_C (18169127256358600576); break;
			case 179: keys [2] = UINT64_C (11896139858948280372); keys [3] = UINT64_C ( 6580558946013887728); break;
			case 180: keys [2] = UINT64_C (15760843106044334771); keys [3] = UINT64_C (17359804073997532575); break;
			case 181: keys [2] = UINT64_C ( 3437204700732782653); keys [3] = UINT64_C ( 3933282017900141422); break;
			case 182: keys [2] = UINT64_C (11950001189362596926); keys [3] = UINT64_C ( 5575180557672791028); break;
			case 183: keys [2] = UINT64_C (14607484645639075381); keys [3] = UINT64_C ( 4364407396080648893); break;
			case 184: keys [2] = UINT64_C (13430363538908230907); keys [3] = UINT64_C ( 6111940459554691935); break;
			case 185: keys [2] = UINT64_C ( 2222659353177641401); keys [3] = UINT64_C (10347512495560462818); break;
			case 186: keys [2] = UINT64_C ( 6530256625445427489); keys [3] = UINT64_C ( 9667375303560635965); break;
			case 187: keys [2] = UINT64_C (14624875965822532593); keys [3] = UINT64_C (14779263307263283925); break;
			case 188: keys [2] = UINT64_C ( 6769393337117578321); keys [3] = UINT64_C ( 7023192903566101398); break;
			case 189: keys [2] = UINT64_C ( 6701802801800933806); keys [3] = UINT64_C (16043830482075391570); break;
			case 190: keys [2] = UINT64_C ( 3297917081476345591); keys [3] = UINT64_C (16055378108294992165); break;
			case 191: keys [2] = UINT64_C (17368087697319574695); keys [3] = UINT64_C ( 8463789284323450360); break;
			case 192: keys [2] = UINT64_C (10063855864370087884); keys [3] = UINT64_C ( 6531339806703112586); break;
			case 193: keys [2] = UINT64_C (17077343882608643146); keys [3] = UINT64_C (18260088433519203329); break;
			case 194: keys [2] = UINT64_C (15066721152840392304); keys [3] = UINT64_C ( 3544651165482261511); break;
			case 195: keys [2] = UINT64_C (14492683588318734156); keys [3] = UINT64_C (11972742223107036712); break;
			case 196: keys [2] = UINT64_C (10748008705133011221); keys [3] = UINT64_C ( 1633195474620427305); break;
			case 197: keys [2] = UINT64_C ( 7432171749986007256); keys [3] = UINT64_C (13094383556626691197); break;
			case 198: keys [2] = UINT64_C ( 5057796214945818580); keys [3] = UINT64_C (16530710037529775882); break;
			case 199: keys [2] = UINT64_C (10676814969357008720); keys [3] = UINT64_C (16505963276191377985); break;
			case 200: keys [2] = UINT64_C (16969688716816900419); keys [3] = UINT64_C ( 4024132414701059950); break;
			case 201: keys [2] = UINT64_C (11566060107990302418); keys [3] = UINT64_C (11424083411254190664); break;
			case 202: keys [2] = UINT64_C ( 8514297471582565209); keys [3] = UINT64_C (18136364849959786838); break;
			case 203: keys [2] = UINT64_C (17304920268849137147); keys [3] = UINT64_C ( 1196021620397185719); break;
			case 204: keys [2] = UINT64_C ( 3267410572582946536); keys [3] = UINT64_C (10617108163798261628); break;
			case 205: keys [2] = UINT64_C ( 8161366027998274927); keys [3] = UINT64_C (10614624445720287224); break;
			case 206: keys [2] = UINT64_C ( 4065368790804221353); keys [3] = UINT64_C ( 4579424654684183677); break;
			case 207: keys [2] = UINT64_C ( 5220401944499755106); keys [3] = UINT64_C (12054786753075369361); break;
			case 208: keys [2] = UINT64_C ( 3578777528132456785); keys [3] = UINT64_C ( 6916492325472643830); break;
			case 209: keys [2] = UINT64_C (16412460156496622712); keys [3] = UINT64_C ( 1051502105149260298); break;
			case 210: keys [2] = UINT64_C ( 2937478703620866562); keys [3] = UINT64_C ( 2031144448684903952); break;
			case 211: keys [2] = UINT64_C (  624208279966862565); keys [3] = UINT64_C ( 6866732350481892265); break;
			case 212: keys [2] = UINT64_C ( 6473659063710517338); keys [3] = UINT64_C ( 2992925889139030496); break;
			case 213: keys [2] = UINT64_C ( 5555156580329579520); keys [3] = UINT64_C ( 8849212028587896438); break;
			case 214: keys [2] = UINT64_C ( 6679935189376183296); keys [3] = UINT64_C (15528016742915617306); break;
			case 215: keys [2] = UINT64_C (12667534113736437334); keys [3] = UINT64_C ( 3187692363010625235); break;
			case 216: keys [2] = UINT64_C ( 9736552042587258980); keys [3] = UINT64_C ( 6259346270701567190); break;
			case 217: keys [2] = UINT64_C (17520871932441068761); keys [3] = UINT64_C ( 5424257954764120118); break;
			case 218: keys [2] = UINT64_C (  780375897278494482); keys [3] = UINT64_C ( 9225525950659796653); break;
			case 219: keys [2] = UINT64_C (18148641324425754567); keys [3] = UINT64_C (10108339813991160918); break;
			case 220: keys [2] = UINT64_C ( 7811652003070073233); keys [3] = UINT64_C ( 8071163199768445400); break;
			case 221: keys [2] = UINT64_C ( 3631462125323791293); keys [3] = UINT64_C ( 1129301892244505427); break;
			case 222: keys [2] = UINT64_C ( 6812806145730028886); keys [3] = UINT64_C (18174018021288046187); break;
			case 223: keys [2] = UINT64_C ( 8924077435812973456); keys [3] = UINT64_C (14524334102112176211); break;
			case 224: keys [2] = UINT64_C ( 2355701903694126949); keys [3] = UINT64_C (11351696180024954914); break;
			case 225: keys [2] = UINT64_C (10082142241573288388); keys [3] = UINT64_C ( 7005491908802477966); break;
			case 226: keys [2] = UINT64_C (12862506059506199524); keys [3] = UINT64_C (16633378455965650203); break;
			case 227: keys [2] = UINT64_C (16946454868601241245); keys [3] = UINT64_C (14571796491193408048); break;
			case 228: keys [2] = UINT64_C (13976836304082559801); keys [3] = UINT64_C ( 7370168488655175700); break;
			case 229: keys [2] = UINT64_C ( 9850548282258578861); keys [3] = UINT64_C ( 6147868687260335784); break;
			case 230: keys [2] = UINT64_C ( 6771242015770713478); keys [3] = UINT64_C ( 6677089388058873209); break;
			case 231: keys [2] = UINT64_C ( 4433943082990790083); keys [3] = UINT64_C ( 3756116313087762900); break;
			case 232: keys [2] = UINT64_C ( 9308563436304539486); keys [3] = UINT64_C ( 2510593830030752158); break;
			case 233: keys [2] = UINT64_C ( 5730287285031517159); keys [3] = UINT64_C ( 3012933236490978067); break;
			case 234: keys [2] = UINT64_C (14490740770507498564); keys [3] = UINT64_C (14102444050671695424); break;
			case 235: keys [2] = UINT64_C ( 6926239208574819520); keys [3] = UINT64_C ( 9097616942263851492); break;
			case 236: keys [2] = UINT64_C ( 1708435975370573836); keys [3] = UINT64_C (  223931650109101474); break;
			case 237: keys [2] = UINT64_C (10299609318909789843); keys [3] = UINT64_C (10491238042880815182); break;
			case 238: keys [2] = UINT64_C (14949597114774291285); keys [3] = UINT64_C ( 8517270772883938350); break;
			case 239: keys [2] = UINT64_C ( 9024426285668186176); keys [3] = UINT64_C ( 1945233520388436321); break;
			case 240: keys [2] = UINT64_C (16285355090348925345); keys [3] = UINT64_C (14348567247656807052); break;
			case 241: keys [2] = UINT64_C (17045095344337754857); keys [3] = UINT64_C (10081637856182774174); break;
			case 242: keys [2] = UINT64_C (  208907753936765335); keys [3] = UINT64_C (12325370087430548380); break;
			case 243: keys [2] = UINT64_C ( 6352652749276334689); keys [3] = UINT64_C ( 4696194253502033857); break;
			case 244: keys [2] = UINT64_C (17555818200301837266); keys [3] = UINT64_C ( 5157224022784200336); break;
			case 245: keys [2] = UINT64_C (13295483388181337842); keys [3] = UINT64_C (12133195313616035066); break;
			case 246: keys [2] = UINT64_C ( 9468835200794175130); keys [3] = UINT64_C ( 9362163001850750544); break;
			case 247: keys [2] = UINT64_C (  290838560300156667); keys [3] = UINT64_C (  932541929351540375); break;
			case 248: keys [2] = UINT64_C (14411386476522064439); keys [3] = UINT64_C ( 1975710277799207941); break;
			case 249: keys [2] = UINT64_C (11255774708022407347); keys [3] = UINT64_C (   53436232107796896); break;
			case 250: keys [2] = UINT64_C ( 9891343068777442619); keys [3] = UINT64_C (15553298224995489264); break;
			case 251: keys [2] = UINT64_C ( 7568110356932722240); keys [3] = UINT64_C ( 3937098633419208278); break;
			case 252: keys [2] = UINT64_C (15022879698044878269); keys [3] = UINT64_C (10853667074937297428); break;
			case 253: keys [2] = UINT64_C (13639791951388031552); keys [3] = UINT64_C ( 3976394619656391340); break;
			case 254: keys [2] = UINT64_C (14812551858632945603); keys [3] = UINT64_C ( 7779709959433938452); break;
			case 255: keys [2] = UINT64_C ( 6487837502290732673); keys [3] = UINT64_C ( 6806032093260030976); break;
			case 256: keys [2] = UINT64_C (16703285416967076001); keys [3] = UINT64_C (11511561181004736890); break;
			case 257: keys [2] = UINT64_C (15028369826959620061); keys [3] = UINT64_C ( 1802263338121185264); break;
			case 258: keys [2] = UINT64_C (10402540834637326234); keys [3] = UINT64_C (  569839609253044302); break;
			case 259: keys [2] = UINT64_C (14803515509365471247); keys [3] = UINT64_C ( 1993940045316459405); break;
			case 260: keys [2] = UINT64_C ( 3780699240841912172); keys [3] = UINT64_C ( 9641271671640143920); break;
			case 261: keys [2] = UINT64_C (11625193093396963211); keys [3] = UINT64_C (13864553363055517982); break;
			case 262: keys [2] = UINT64_C ( 3906408402733230872); keys [3] = UINT64_C (12575231688589745232); break;
			case 263: keys [2] = UINT64_C ( 4761473334064968892); keys [3] = UINT64_C ( 5122445475552046722); break;
			case 264: keys [2] = UINT64_C (15700510942443834251); keys [3] = UINT64_C ( 6188157897434576573); break;
			case 265: keys [2] = UINT64_C ( 8795655837703686457); keys [3] = UINT64_C ( 9423826681799276998); break;
			case 266: keys [2] = UINT64_C (14170079537983874867); keys [3] = UINT64_C (  872548508682509402); break;
			case 267: keys [2] = UINT64_C (17236084833637523081); keys [3] = UINT64_C (12518104622778258023); break;
			case 268: keys [2] = UINT64_C (12258188833680893506); keys [3] = UINT64_C ( 8104666117998115409); break;
			case 269: keys [2] = UINT64_C (16188058688294352308); keys [3] = UINT64_C (13104149983718654266); break;
			case 270: keys [2] = UINT64_C ( 7589745174447041078); keys [3] = UINT64_C ( 3077067914129179224); break;
			case 271: keys [2] = UINT64_C (15001869584470422961); keys [3] = UINT64_C (12956900915183097078); break;
			case 272: keys [2] = UINT64_C (15118862549805555641); keys [3] = UINT64_C ( 2839282945209453081); break;
			case 273: keys [2] = UINT64_C (12525606049247034515); keys [3] = UINT64_C ( 3159339132146328678); break;
			case 274: keys [2] = UINT64_C (14537723237009683926); keys [3] = UINT64_C (17983866530820859628); break;
			case 275: keys [2] = UINT64_C (13834930966398787492); keys [3] = UINT64_C (11685808535213274042); break;
			case 276: keys [2] = UINT64_C ( 3017405914512873155); keys [3] = UINT64_C ( 6211789502582542122); break;
			case 277: keys [2] = UINT64_C ( 2049713954686766353); keys [3] = UINT64_C (12892937470761307906); break;
			case 278: keys [2] = UINT64_C ( 4433452151456399479); keys [3] = UINT64_C ( 6869271381670182705); break;
			case 279: keys [2] = UINT64_C (18384895288323928862); keys [3] = UINT64_C (17130590165992653481); break;
			case 280: keys [2] = UINT64_C (17010457226923106571); keys [3] = UINT64_C (  850683844192379329); break;
			case 281: keys [2] = UINT64_C ( 5911115477059574685); keys [3] = UINT64_C ( 4119038902560260602); break;
			case 282: keys [2] = UINT64_C ( 7813887387576958770); keys [3] = UINT64_C ( 6836475990413327743); break;
			case 283: keys [2] = UINT64_C (15743207894773422596); keys [3] = UINT64_C ( 8672874688213408437); break;
			case 284: keys [2] = UINT64_C (15989025911373793080); keys [3] = UINT64_C (13097197725792194265); break;
			case 285: keys [2] = UINT64_C ( 1205376118351609098); keys [3] = UINT64_C (17052986536857879502); break;
			case 286: keys [2] = UINT64_C (15287306481282822988); keys [3] = UINT64_C ( 5406408606809524541); break;
			case 287: keys [2] = UINT64_C (18291376253194646277); keys [3] = UINT64_C ( 9738379114399873223); break;
			case 288: keys [2] = UINT64_C ( 4606815519299246784); keys [3] = UINT64_C ( 7113215467857130724); break;
			case 289: keys [2] = UINT64_C (13215689435241277130); keys [3] = UINT64_C (11818321012623744453); break;
			case 290: keys [2] = UINT64_C ( 8578110448049358247); keys [3] = UINT64_C (11118279271114123345); break;
			case 291: keys [2] = UINT64_C (10899137878132320227); keys [3] = UINT64_C (10469367272775777341); break;
			case 292: keys [2] = UINT64_C (15439771499241070197); keys [3] = UINT64_C (  751272300206668333); break;
			case 293: keys [2] = UINT64_C ( 4338630200342140688); keys [3] = UINT64_C ( 5403045752046677754); break;
			case 294: keys [2] = UINT64_C (16977831441983421899); keys [3] = UINT64_C (14212162320292649628); break;
			case 295: keys [2] = UINT64_C ( 4507428508027600545); keys [3] = UINT64_C (11142066564904664337); break;
			case 296: keys [2] = UINT64_C (11380386809499754831); keys [3] = UINT64_C (10409061275752712263); break;
			case 297: keys [2] = UINT64_C ( 1576477000119414701); keys [3] = UINT64_C ( 4649065822708314228); break;
			case 298: keys [2] = UINT64_C (14354842370142354909); keys [3] = UINT64_C (11414687325450345241); break;
			case 299: keys [2] = UINT64_C (13937343786652392567); keys [3] = UINT64_C (17741007563456682866); break;
			case 300: keys [2] = UINT64_C ( 1171513968993481720); keys [3] = UINT64_C (16829728667897953076); break;
			case 301: keys [2] = UINT64_C ( 4562941155467549111); keys [3] = UINT64_C (  897181573878040084); break;
			case 302: keys [2] = UINT64_C (15240797935111252417); keys [3] = UINT64_C (14134564121530782338); break;
			case 303: keys [2] = UINT64_C ( 5663108495006454419); keys [3] = UINT64_C (12165073752063968602); break;
			case 304: keys [2] = UINT64_C ( 7851106410208604251); keys [3] = UINT64_C ( 2113229121453432903); break;
			case 305: keys [2] = UINT64_C (11707064298313491252); keys [3] = UINT64_C ( 3875372582858434303); break;
			case 306: keys [2] = UINT64_C (12190598090805764800); keys [3] = UINT64_C (16971369040396471277); break;
			case 307: keys [2] = UINT64_C (15842220132009893786); keys [3] = UINT64_C (17612515037781198250); break;
			case 308: keys [2] = UINT64_C ( 3583539186432569713); keys [3] = UINT64_C (13813474840301837597); break;
			case 309: keys [2] = UINT64_C ( 9778734044608970930); keys [3] = UINT64_C ( 9543575657384869357); break;
			case 310: keys [2] = UINT64_C (13799502839527770207); keys [3] = UINT64_C (13962832120919851093); break;
			case 311: keys [2] = UINT64_C (16559740097197135213); keys [3] = UINT64_C (12884069972689240725); break;
			case 312: keys [2] = UINT64_C (15943699761517143567); keys [3] = UINT64_C ( 3374978250331850970); break;
			case 313: keys [2] = UINT64_C ( 9210506667518353302); keys [3] = UINT64_C ( 6699634545029756530); break;
			case 314: keys [2] = UINT64_C (18264877918096545941); keys [3] = UINT64_C ( 1597338360383885593); break;
			case 315: keys [2] = UINT64_C (14479546524376236413); keys [3] = UINT64_C (16949784267196960442); break;
			case 316: keys [2] = UINT64_C (  804273443622144121); keys [3] = UINT64_C (  977339474562557309); break;
			case 317: keys [2] = UINT64_C (13761847305156367572); keys [3] = UINT64_C (10300933256410541467); break;
			case 318: keys [2] = UINT64_C (17082293467432144244); keys [3] = UINT64_C (16200747776704093611); break;
			case 319: keys [2] = UINT64_C (17479192726331733728); keys [3] = UINT64_C ( 4506485022458819523); break;
			case 320: keys [2] = UINT64_C (12120288860308139425); keys [3] = UINT64_C (11785959114584963137); break;
			case 321: keys [2] = UINT64_C (13386696323900227278); keys [3] = UINT64_C ( 9384767783048125977); break;
			case 322: keys [2] = UINT64_C ( 6400548965532457033); keys [3] = UINT64_C (11716593078023009708); break;
			case 323: keys [2] = UINT64_C (18265519998776190680); keys [3] = UINT64_C (13062410941839245045); break;
			case 324: keys [2] = UINT64_C (   37551894587222764); keys [3] = UINT64_C ( 5342208361147332174); break;
			case 325: keys [2] = UINT64_C (14567376482852783954); keys [3] = UINT64_C (15278150799098962162); break;
			case 326: keys [2] = UINT64_C (10801344474124975767); keys [3] = UINT64_C (12211135441695570454); break;
			case 327: keys [2] = UINT64_C (11020478861126930828); keys [3] = UINT64_C (  406278814038529270); break;
			case 328: keys [2] = UINT64_C (12415270765845175619); keys [3] = UINT64_C ( 4118428027479171750); break;
			case 329: keys [2] = UINT64_C ( 4224912257269588064); keys [3] = UINT64_C (11363822588174348994); break;
			case 330: keys [2] = UINT64_C ( 2081681298700017761); keys [3] = UINT64_C ( 9310637385091328476); break;
			case 331: keys [2] = UINT64_C (11173440547597634027); keys [3] = UINT64_C (13024070691433765550); break;
			case 332: keys [2] = UINT64_C (12379269748295918835); keys [3] = UINT64_C ( 5048267403701106105); break;
			case 333: keys [2] = UINT64_C (13431682445371013878); keys [3] = UINT64_C (12969540247340618755); break;
			case 334: keys [2] = UINT64_C ( 7860536391320575013); keys [3] = UINT64_C (10477308332196506597); break;
			case 335: keys [2] = UINT64_C ( 1756181583222695402); keys [3] = UINT64_C ( 7862207505419163035); break;
			case 336: keys [2] = UINT64_C (11940426784789446021); keys [3] = UINT64_C ( 8701623604841333930); break;
			case 337: keys [2] = UINT64_C (14441447615203299981); keys [3] = UINT64_C (17618981969365351625); break;
			case 338: keys [2] = UINT64_C (13044980762211218505); keys [3] = UINT64_C (12587750423225144338); break;
			case 339: keys [2] = UINT64_C (17924302025107946313); keys [3] = UINT64_C (16790480554941023541); break;
			case 340: keys [2] = UINT64_C ( 6298683117241598653); keys [3] = UINT64_C ( 2610574565192749071); break;
			case 341: keys [2] = UINT64_C ( 3420827379702875515); keys [3] = UINT64_C ( 7004114776016896415); break;
			case 342: keys [2] = UINT64_C ( 3410357381662358049); keys [3] = UINT64_C ( 9833825427996465968); break;
			case 343: keys [2] = UINT64_C (10707446731314465601); keys [3] = UINT64_C (18152527962423025397); break;
			case 344: keys [2] = UINT64_C ( 9467472556544554291); keys [3] = UINT64_C (12908852245821309104); break;
			case 345: keys [2] = UINT64_C (10039526147468314577); keys [3] = UINT64_C (15169567147467958220); break;
			case 346: keys [2] = UINT64_C ( 1876751105237095878); keys [3] = UINT64_C (10742000014047667868); break;
			case 347: keys [2] = UINT64_C (15409750648415339819); keys [3] = UINT64_C ( 4633948822022171569); break;
			case 348: keys [2] = UINT64_C (17418195256457005923); keys [3] = UINT64_C ( 9469400975798202786); break;
			case 349: keys [2] = UINT64_C (17697425992707698270); keys [3] = UINT64_C ( 5870153754208808007); break;
			case 350: keys [2] = UINT64_C (18322353631348709359); keys [3] = UINT64_C ( 3347913481264049421); break;
			case 351: keys [2] = UINT64_C (14144129659510363310); keys [3] = UINT64_C ( 9161529410263087732); break;
			case 352: keys [2] = UINT64_C (15448241305710211035); keys [3] = UINT64_C ( 4929079050029201415); break;
			case 353: keys [2] = UINT64_C ( 9938792124925782129); keys [3] = UINT64_C (14785200632503179278); break;
			case 354: keys [2] = UINT64_C (16077377155219708422); keys [3] = UINT64_C (14531291285520446807); break;
			case 355: keys [2] = UINT64_C (18331386811390767903); keys [3] = UINT64_C (18161464609299925730); break;
			case 356: keys [2] = UINT64_C ( 6139653386181215457); keys [3] = UINT64_C ( 1521185471024249568); break;
			case 357: keys [2] = UINT64_C ( 4840917016562399236); keys [3] = UINT64_C (13471472110222884183); break;
			case 358: keys [2] = UINT64_C (10049828597055100420); keys [3] = UINT64_C ( 9404887764326171697); break;
			case 359: keys [2] = UINT64_C ( 2309896771802191463); keys [3] = UINT64_C (17433662001895038733); break;
			case 360: keys [2] = UINT64_C ( 9102561156061675969); keys [3] = UINT64_C ( 3006706699168419312); break;
			case 361: keys [2] = UINT64_C (17356853214262051723); keys [3] = UINT64_C (12714009987506277964); break;
			case 362: keys [2] = UINT64_C (13042225292754400832); keys [3] = UINT64_C (12708390266547658939); break;
			case 363: keys [2] = UINT64_C ( 2127342965049920288); keys [3] = UINT64_C ( 7207132188136429338); break;
			case 364: keys [2] = UINT64_C ( 2145223265868264279); keys [3] = UINT64_C ( 8250703040329140080); break;
			case 365: keys [2] = UINT64_C ( 9585449627180450039); keys [3] = UINT64_C ( 5304320490145559787); break;
			case 366: keys [2] = UINT64_C (13896262271419841200); keys [3] = UINT64_C (13345476412532758353); break;
			case 367: keys [2] = UINT64_C ( 5206054343055542485); keys [3] = UINT64_C ( 6934773796890833931); break;
			case 368: keys [2] = UINT64_C ( 4183777809320350640); keys [3] = UINT64_C ( 9282096342964463254); break;
			case 369: keys [2] = UINT64_C (16941410006283320760); keys [3] = UINT64_C ( 8609712948597145551); break;
			case 370: keys [2] = UINT64_C ( 9086571052453144728); keys [3] = UINT64_C (13847766715374396357); break;
			case 371: keys [2] = UINT64_C (12470480309003395415); keys [3] = UINT64_C ( 1397493571059672883); break;
			case 372: keys [2] = UINT64_C (14971410447687270965); keys [3] = UINT64_C (  759907703386305467); break;
			case 373: keys [2] = UINT64_C (13080321959947498106); keys [3] = UINT64_C ( 2267632466545318526); break;
			case 374: keys [2] = UINT64_C ( 5253076699025465917); keys [3] = UINT64_C ( 4040277653847269201); break;
			case 375: keys [2] = UINT64_C (  337015115307866318); keys [3] = UINT64_C (11617396875368506926); break;
			case 376: keys [2] = UINT64_C (17378598177108208181); keys [3] = UINT64_C (14864151914656353245); break;
			case 377: keys [2] = UINT64_C ( 8675103762015636011); keys [3] = UINT64_C (13520746309257087599); break;
			case 378: keys [2] = UINT64_C ( 1791100160307646673); keys [3] = UINT64_C (11417642400375864988); break;
			case 379: keys [2] = UINT64_C (11438730428992662639); keys [3] = UINT64_C ( 6811715991751761315); break;
			case 380: keys [2] = UINT64_C ( 1010103033510756562); keys [3] = UINT64_C ( 6026491738613508384); break;
			case 381: keys [2] = UINT64_C ( 6147020050874267371); keys [3] = UINT64_C (12446440787339761327); break;
			case 382: keys [2] = UINT64_C ( 9256855293947002185); keys [3] = UINT64_C ( 5462036622964502357); break;
			case 383: keys [2] = UINT64_C (10734534653512108950); keys [3] = UINT64_C ( 2156113376459065218); break;
			case 384: keys [2] = UINT64_C ( 2132758186693730467); keys [3] = UINT64_C ( 4766980551347159988); break;
			case 385: keys [2] = UINT64_C (17446897886677896382); keys [3] = UINT64_C ( 3405261035555657113); break;
			case 386: keys [2] = UINT64_C ( 1588760379309047443); keys [3] = UINT64_C ( 9151216989497274945); break;
			case 387: keys [2] = UINT64_C ( 4455000861113825995); keys [3] = UINT64_C (14196529188433783767); break;
			case 388: keys [2] = UINT64_C ( 3846053476994585867); keys [3] = UINT64_C ( 2234299894044673928); break;
			case 389: keys [2] = UINT64_C ( 1818283822156461423); keys [3] = UINT64_C ( 9485502107013757986); break;
			case 390: keys [2] = UINT64_C (11292128494905056306); keys [3] = UINT64_C ( 1829636055587051972); break;
			case 391: keys [2] = UINT64_C (18061947396837945144); keys [3] = UINT64_C (12312040185562716654); break;
			case 392: keys [2] = UINT64_C (17421148285593362560); keys [3] = UINT64_C ( 1387245036525645394); break;
			case 393: keys [2] = UINT64_C ( 7511498218496563164); keys [3] = UINT64_C ( 7310032071509747647); break;
			case 394: keys [2] = UINT64_C ( 5917878417858457697); keys [3] = UINT64_C ( 2444508867814296315); break;
			case 395: keys [2] = UINT64_C (  139904915607487835); keys [3] = UINT64_C (10452129402493395366); break;
			case 396: keys [2] = UINT64_C ( 2258263582747280186); keys [3] = UINT64_C (  441969801308861439); break;
			case 397: keys [2] = UINT64_C (13008490726590060528); keys [3] = UINT64_C ( 2221468865257816501); break;
			case 398: keys [2] = UINT64_C (11925485659471541722); keys [3] = UINT64_C (14425553645651431410); break;
			case 399: keys [2] = UINT64_C (16491421204212301836); keys [3] = UINT64_C (14045414575791108036); break;
			case 400: keys [2] = UINT64_C ( 2547444441667548194); keys [3] = UINT64_C ( 7931822093344929483); break;
			case 401: keys [2] = UINT64_C (17831319740718139641); keys [3] = UINT64_C ( 1702294398737654838); break;
			case 402: keys [2] = UINT64_C (  994730030552399391); keys [3] = UINT64_C ( 8090595461759216181); break;
			default: Melder_crash (U"Thread number too high.");
		}
		keys [4] = (uint64) (int64) getpid ();   // unique between processes that run simultaneously on the same computer
		keys [5] = ticksSinceBoot;   // some extra randomness
		static uint64 callInstance = 0;
		keys [6] = UINT64_C (3642334578453) + (++ callInstance);
		states [ichan]. init_by_array64 (keys, numberOfKeys);
	}
	theInited = true;
	//fprintf (stderr, "%f", Melder_clock() - startingTime);   // in case we want to optimize start-up, but it's only 1 ms
}
void NUMrandom_initializeWithSeedUnsafelyButPredictably (uint64 seed) {
	for (integer ichan = 0; ichan < NUMrandom_numberOfChannels; ichan ++)
		seed = states [ichan]. init_genrand64 (seed + (uint64) ichan);
	theInited = true;
}

/* Throughout the years, several versions for "zero or magic" have been proposed. Choose the fastest. */

#define ZERO_OR_MAGIC_VERSION  3

#if ZERO_OR_MAGIC_VERSION == 1   // M&N 1999
	#define ZERO_OR_MAGIC  ( (x & UINT64_C (1)) ? MATRIX_A : UINT64_C (0) )
#elif ZERO_OR_MAGIC_VERSION == 2
	#define ZERO_OR_MAGIC  ( (x & UINT64_C (1)) * MATRIX_A )
#else   // M&N 2014
	constexpr uint64 mag01 [2] { UINT64_C (0), MATRIX_A };
	#define ZERO_OR_MAGIC  mag01 [(int) (x & UINT64_C (1))]
#endif

static thread_local integer theRandomChannel { 0 };

void NUMrandom_setChannel (integer channelNumber) {
	Melder_assert (channelNumber >= 0 && channelNumber < NUMrandom_numberOfChannels);
	theRandomChannel = channelNumber;
}
void NUMrandom_useUserInterfaceChannel () {
	theRandomChannel = 401;
}
void NUMrandom_useSpareChannel () {
	theRandomChannel = 402;
}

// To be set by each parallel thread to a value between 0 and NUMrandom_maximumNumberOfParallelThreads - 1,
// or by special processes to a value between 0 and NUMrandom_numberOfChannels - 1
integer NUMrandom_getChannel () {
	return theRandomChannel;
}

double NUMrandomFraction () {
	NUMrandom_State *me = & states [theRandomChannel];
	uint64 x;

	if (my index >= NN) {   // generate NN words at a time

		Melder_assert (theInited);   // if NUMrandom_initXXX() hasn't been called, we'll detect that here, probably in the first call

		int i;
		for (i = 0; i < NN - MM; i ++) {
			x = (my array [i] & UM) | (my array [i + 1] & LM);
			my array [i] = my array [i + MM] ^ (x >> 1) ^ ZERO_OR_MAGIC;
		}
		for (; i < NN - 1; i ++) {
			x = (my array [i] & UM) | (my array [i + 1] & LM);
			my array [i] = my array [i + (MM - NN)] ^ (x >> 1) ^ ZERO_OR_MAGIC;
		}
		x = (my array [NN - 1] & UM) | (my array [0] & LM);
		my array [NN - 1] = my array [MM - 1] ^ (x >> 1) ^ ZERO_OR_MAGIC;

		my index = 0;
	}

	x = my array [my index ++];

	x ^= (x >> 29) & UINT64_C (0x5555555555555555);
	x ^= (x << 17) & UINT64_C (0x71D67FFFEDA60000);
	x ^= (x << 37) & UINT64_C (0xFFF7EEE000000000);
	x ^= (x >> 43);

	return (x >> 11) * (1.0/9007199254740992.0);
}

double NUMrandomUniform (double lowest, double highest) {
	return lowest + (highest - lowest) * NUMrandomFraction ();
}

integer NUMrandomInteger (integer lowest, integer highest) {
	return lowest + (integer) ((highest - lowest + 1) * NUMrandomFraction ());   // round down by truncation, because positive
}

bool NUMrandomBernoulli (double probability) {
	return NUMrandomFraction() < probability;
}

double NUMrandomBernoulli_real (double probability) {
	return (double) (NUMrandomFraction() < probability);
}

#define repeat  do
#define until(cond)  while (! (cond))
double NUMrandomGauss (double mean, double standardDeviation) {
	NUMrandom_State *me = & states [theRandomChannel];
	/*
		Knuth, p. 122.
	*/
	if (my secondAvailable) {
		my secondAvailable = false;
		return mean + standardDeviation * my y;
	} else {
		double s, x;
		repeat {
			x = 2.0 * NUMrandomFraction () - 1.0;   // inside the square [-1; 1] x [-1; 1]
			my y = 2.0 * NUMrandomFraction () - 1.0;
			s = x * x + my y * my y;
		} until (s < 1.0);   // inside the unit circle
		if (s == 0.0) {
			x = my y = 0.0;
		} else {
			const double factor = sqrt (-2.0 * log (s) / s);
			x *= factor;
			my y *= factor;
		}
		my secondAvailable = true;
		return mean + standardDeviation * x;
	}
}

double NUMrandomPoisson (double mean) {
	/*
		The Poisson distribution is

			P(k) = mean^k * exp (- mean) / k!

		We have to find a function, with known primitive,
		that is always (a bit) greater than P (k).
		This function is based on the Lorentzian distribution,
		with a maximum of P(mean)/0.9 at k=mean:

			f (k) = mean^mean * exp (- mean) / mean! / (0.9 * (1 + (k - mean)^2 / (2 * mean)))

		The tangent is computed as the deviate

			tangent = tan (pi * unif (0, 1))

		This must equal the square root of (k - mean)^2 / (2 * mean),
		so that a trial value for k is given by

			k = floor (mean + tangent * sqrt (2 * mean))

		The probability that this is a good value is proportionate to the ratio of the Poisson
		distribution and the encapsulating function:

			probability = P (k) / f (k) = 0.9 * (1 + tangent^2) * mean ^ (k - mean) * mean! / k!

		The last two factors can be calculated as

			exp ((k - mean) * ln (mean) + lnGamma (mean + 1) - lnGamma (k + 1))
	*/
	static double previousMean = -1.0;   // this routine may well be called repeatedly with the same mean; optimize
	if (mean < 8.0) {
		static double expMean;
		double product = 1.0;
		integer result = -1;
		if (mean != previousMean) {
			previousMean = mean;
			expMean = exp (- mean);
		}
		repeat {
			product *= NUMrandomFraction ();
			result ++;
		} until (product <= expMean);
		return result;
	} else {
		static double sqrtTwoMean, lnMean, lnMeanFactorial;
		double result, probability, tangent;
		if (mean != previousMean) {
			previousMean = mean;
			sqrtTwoMean = sqrt (2.0 * mean);
			lnMean = log (mean);
			lnMeanFactorial = NUMlnGamma (mean + 1.0);
		}
		repeat {
			repeat {
				tangent = tan (NUMpi * NUMrandomFraction ());
				result = mean + tangent * sqrtTwoMean;
			} until (result >= 0.0);
			result = floor (result);
			probability = 0.9 * (1.0 + tangent * tangent) * exp ((result - mean) * lnMean + lnMeanFactorial - NUMlnGamma (result + 1.0));
		} until (NUMrandomFraction () <= probability);
		return result;
	}
}

uint32 NUMhashString (conststring32 string) {
	/*
	 * Jenkins' one-at-a-time hash.
	 */
	uint32 hash = 0;
	for (char32 kar = *string; kar != U'\0'; kar = * (++ string)) {
		hash += (kar >> 16) & 0xFF;   // highest five bits (a char32 actually has only 21 significant bits)
		hash += (hash << 10);
		hash ^= (hash >> 6);
		hash += (kar >> 8) & 0xFF;   // middle 8 bits
		hash += (hash << 10);
		hash ^= (hash >> 6);
		hash += kar & 0xFF;   // lowest 8 bits
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return hash;
}

/* End of file NUMrandom.cpp */
