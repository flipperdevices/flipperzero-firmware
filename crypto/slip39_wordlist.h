/**
 * This file is part of the TREZOR project, https://trezor.io/
 *
 * Copyright (c) SatoshiLabs
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef __SLIP39_WORDLIST_H__
#define __SLIP39_WORDLIST_H__

#include <stdint.h>

#define WORDS_COUNT 1024

static const char* const slip39_wordlist[WORDS_COUNT] = {
    "academic", "acid",     "acne",     "acquire",  "acrobat",  "activity",
    "actress",  "adapt",    "adequate", "adjust",   "admit",    "adorn",
    "adult",    "advance",  "advocate", "afraid",   "again",    "agency",
    "agree",    "aide",     "aircraft", "airline",  "airport",  "ajar",
    "alarm",    "album",    "alcohol",  "alien",    "alive",    "alpha",
    "already",  "alto",     "aluminum", "always",   "amazing",  "ambition",
    "amount",   "amuse",    "analysis", "anatomy",  "ancestor", "ancient",
    "angel",    "angry",    "animal",   "answer",   "antenna",  "anxiety",
    "apart",    "aquatic",  "arcade",   "arena",    "argue",    "armed",
    "artist",   "artwork",  "aspect",   "auction",  "august",   "aunt",
    "average",  "aviation", "avoid",    "award",    "away",     "axis",
    "axle",     "beam",     "beard",    "beaver",   "become",   "bedroom",
    "behavior", "being",    "believe",  "belong",   "benefit",  "best",
    "beyond",   "bike",     "biology",  "birthday", "bishop",   "black",
    "blanket",  "blessing", "blimp",    "blind",    "blue",     "body",
    "bolt",     "boring",   "born",     "both",     "boundary", "bracelet",
    "branch",   "brave",    "breathe",  "briefing", "broken",   "brother",
    "browser",  "bucket",   "budget",   "building", "bulb",     "bulge",
    "bumpy",    "bundle",   "burden",   "burning",  "busy",     "buyer",
    "cage",     "calcium",  "camera",   "campus",   "canyon",   "capacity",
    "capital",  "capture",  "carbon",   "cards",    "careful",  "cargo",
    "carpet",   "carve",    "category", "cause",    "ceiling",  "center",
    "ceramic",  "champion", "change",   "charity",  "check",    "chemical",
    "chest",    "chew",     "chubby",   "cinema",   "civil",    "class",
    "clay",     "cleanup",  "client",   "climate",  "clinic",   "clock",
    "clogs",    "closet",   "clothes",  "club",     "cluster",  "coal",
    "coastal",  "coding",   "column",   "company",  "corner",   "costume",
    "counter",  "course",   "cover",    "cowboy",   "cradle",   "craft",
    "crazy",    "credit",   "cricket",  "criminal", "crisis",   "critical",
    "crowd",    "crucial",  "crunch",   "crush",    "crystal",  "cubic",
    "cultural", "curious",  "curly",    "custody",  "cylinder", "daisy",
    "damage",   "dance",    "darkness", "database", "daughter", "deadline",
    "deal",     "debris",   "debut",    "decent",   "decision", "declare",
    "decorate", "decrease", "deliver",  "demand",   "density",  "deny",
    "depart",   "depend",   "depict",   "deploy",   "describe", "desert",
    "desire",   "desktop",  "destroy",  "detailed", "detect",   "device",
    "devote",   "diagnose", "dictate",  "diet",     "dilemma",  "diminish",
    "dining",   "diploma",  "disaster", "discuss",  "disease",  "dish",
    "dismiss",  "display",  "distance", "dive",     "divorce",  "document",
    "domain",   "domestic", "dominant", "dough",    "downtown", "dragon",
    "dramatic", "dream",    "dress",    "drift",    "drink",    "drove",
    "drug",     "dryer",    "duckling", "duke",     "duration", "dwarf",
    "dynamic",  "early",    "earth",    "easel",    "easy",     "echo",
    "eclipse",  "ecology",  "edge",     "editor",   "educate",  "either",
    "elbow",    "elder",    "election", "elegant",  "element",  "elephant",
    "elevator", "elite",    "else",     "email",    "emerald",  "emission",
    "emperor",  "emphasis", "employer", "empty",    "ending",   "endless",
    "endorse",  "enemy",    "energy",   "enforce",  "engage",   "enjoy",
    "enlarge",  "entrance", "envelope", "envy",     "epidemic", "episode",
    "equation", "equip",    "eraser",   "erode",    "escape",   "estate",
    "estimate", "evaluate", "evening",  "evidence", "evil",     "evoke",
    "exact",    "example",  "exceed",   "exchange", "exclude",  "excuse",
    "execute",  "exercise", "exhaust",  "exotic",   "expand",   "expect",
    "explain",  "express",  "extend",   "extra",    "eyebrow",  "facility",
    "fact",     "failure",  "faint",    "fake",     "false",    "family",
    "famous",   "fancy",    "fangs",    "fantasy",  "fatal",    "fatigue",
    "favorite", "fawn",     "fiber",    "fiction",  "filter",   "finance",
    "findings", "finger",   "firefly",  "firm",     "fiscal",   "fishing",
    "fitness",  "flame",    "flash",    "flavor",   "flea",     "flexible",
    "flip",     "float",    "floral",   "fluff",    "focus",    "forbid",
    "force",    "forecast", "forget",   "formal",   "fortune",  "forward",
    "founder",  "fraction", "fragment", "frequent", "freshman", "friar",
    "fridge",   "friendly", "frost",    "froth",    "frozen",   "fumes",
    "funding",  "furl",     "fused",    "galaxy",   "game",     "garbage",
    "garden",   "garlic",   "gasoline", "gather",   "general",  "genius",
    "genre",    "genuine",  "geology",  "gesture",  "glad",     "glance",
    "glasses",  "glen",     "glimpse",  "goat",     "golden",   "graduate",
    "grant",    "grasp",    "gravity",  "gray",     "greatest", "grief",
    "grill",    "grin",     "grocery",  "gross",    "group",    "grownup",
    "grumpy",   "guard",    "guest",    "guilt",    "guitar",   "gums",
    "hairy",    "hamster",  "hand",     "hanger",   "harvest",  "have",
    "havoc",    "hawk",     "hazard",   "headset",  "health",   "hearing",
    "heat",     "helpful",  "herald",   "herd",     "hesitate", "hobo",
    "holiday",  "holy",     "home",     "hormone",  "hospital", "hour",
    "huge",     "human",    "humidity", "hunting",  "husband",  "hush",
    "husky",    "hybrid",   "idea",     "identify", "idle",     "image",
    "impact",   "imply",    "improve",  "impulse",  "include",  "income",
    "increase", "index",    "indicate", "industry", "infant",   "inform",
    "inherit",  "injury",   "inmate",   "insect",   "inside",   "install",
    "intend",   "intimate", "invasion", "involve",  "iris",     "island",
    "isolate",  "item",     "ivory",    "jacket",   "jerky",    "jewelry",
    "join",     "judicial", "juice",    "jump",     "junction", "junior",
    "junk",     "jury",     "justice",  "kernel",   "keyboard", "kidney",
    "kind",     "kitchen",  "knife",    "knit",     "laden",    "ladle",
    "ladybug",  "lair",     "lamp",     "language", "large",    "laser",
    "laundry",  "lawsuit",  "leader",   "leaf",     "learn",    "leaves",
    "lecture",  "legal",    "legend",   "legs",     "lend",     "length",
    "level",    "liberty",  "library",  "license",  "lift",     "likely",
    "lilac",    "lily",     "lips",     "liquid",   "listen",   "literary",
    "living",   "lizard",   "loan",     "lobe",     "location", "losing",
    "loud",     "loyalty",  "luck",     "lunar",    "lunch",    "lungs",
    "luxury",   "lying",    "lyrics",   "machine",  "magazine", "maiden",
    "mailman",  "main",     "makeup",   "making",   "mama",     "manager",
    "mandate",  "mansion",  "manual",   "marathon", "march",    "market",
    "marvel",   "mason",    "material", "math",     "maximum",  "mayor",
    "meaning",  "medal",    "medical",  "member",   "memory",   "mental",
    "merchant", "merit",    "method",   "metric",   "midst",    "mild",
    "military", "mineral",  "minister", "miracle",  "mixed",    "mixture",
    "mobile",   "modern",   "modify",   "moisture", "moment",   "morning",
    "mortgage", "mother",   "mountain", "mouse",    "move",     "much",
    "mule",     "multiple", "muscle",   "museum",   "music",    "mustang",
    "nail",     "national", "necklace", "negative", "nervous",  "network",
    "news",     "nuclear",  "numb",     "numerous", "nylon",    "oasis",
    "obesity",  "object",   "observe",  "obtain",   "ocean",    "often",
    "olympic",  "omit",     "oral",     "orange",   "orbit",    "order",
    "ordinary", "organize", "ounce",    "oven",     "overall",  "owner",
    "paces",    "pacific",  "package",  "paid",     "painting", "pajamas",
    "pancake",  "pants",    "papa",     "paper",    "parcel",   "parking",
    "party",    "patent",   "patrol",   "payment",  "payroll",  "peaceful",
    "peanut",   "peasant",  "pecan",    "penalty",  "pencil",   "percent",
    "perfect",  "permit",   "petition", "phantom",  "pharmacy", "photo",
    "phrase",   "physics",  "pickup",   "picture",  "piece",    "pile",
    "pink",     "pipeline", "pistol",   "pitch",    "plains",   "plan",
    "plastic",  "platform", "playoff",  "pleasure", "plot",     "plunge",
    "practice", "prayer",   "preach",   "predator", "pregnant", "premium",
    "prepare",  "presence", "prevent",  "priest",   "primary",  "priority",
    "prisoner", "privacy",  "prize",    "problem",  "process",  "profile",
    "program",  "promise",  "prospect", "provide",  "prune",    "public",
    "pulse",    "pumps",    "punish",   "puny",     "pupal",    "purchase",
    "purple",   "python",   "quantity", "quarter",  "quick",    "quiet",
    "race",     "racism",   "radar",    "railroad", "rainbow",  "raisin",
    "random",   "ranked",   "rapids",   "raspy",    "reaction", "realize",
    "rebound",  "rebuild",  "recall",   "receiver", "recover",  "regret",
    "regular",  "reject",   "relate",   "remember", "remind",   "remove",
    "render",   "repair",   "repeat",   "replace",  "require",  "rescue",
    "research", "resident", "response", "result",   "retailer", "retreat",
    "reunion",  "revenue",  "review",   "reward",   "rhyme",    "rhythm",
    "rich",     "rival",    "river",    "robin",    "rocky",    "romantic",
    "romp",     "roster",   "round",    "royal",    "ruin",     "ruler",
    "rumor",    "sack",     "safari",   "salary",   "salon",    "salt",
    "satisfy",  "satoshi",  "saver",    "says",     "scandal",  "scared",
    "scatter",  "scene",    "scholar",  "science",  "scout",    "scramble",
    "screw",    "script",   "scroll",   "seafood",  "season",   "secret",
    "security", "segment",  "senior",   "shadow",   "shaft",    "shame",
    "shaped",   "sharp",    "shelter",  "sheriff",  "short",    "should",
    "shrimp",   "sidewalk", "silent",   "silver",   "similar",  "simple",
    "single",   "sister",   "skin",     "skunk",    "slap",     "slavery",
    "sled",     "slice",    "slim",     "slow",     "slush",    "smart",
    "smear",    "smell",    "smirk",    "smith",    "smoking",  "smug",
    "snake",    "snapshot", "sniff",    "society",  "software", "soldier",
    "solution", "soul",     "source",   "space",    "spark",    "speak",
    "species",  "spelling", "spend",    "spew",     "spider",   "spill",
    "spine",    "spirit",   "spit",     "spray",    "sprinkle", "square",
    "squeeze",  "stadium",  "staff",    "standard", "starting", "station",
    "stay",     "steady",   "step",     "stick",    "stilt",    "story",
    "strategy", "strike",   "style",    "subject",  "submit",   "sugar",
    "suitable", "sunlight", "superior", "surface",  "surprise", "survive",
    "sweater",  "swimming", "swing",    "switch",   "symbolic", "sympathy",
    "syndrome", "system",   "tackle",   "tactics",  "tadpole",  "talent",
    "task",     "taste",    "taught",   "taxi",     "teacher",  "teammate",
    "teaspoon", "temple",   "tenant",   "tendency", "tension",  "terminal",
    "testify",  "texture",  "thank",    "that",     "theater",  "theory",
    "therapy",  "thorn",    "threaten", "thumb",    "thunder",  "ticket",
    "tidy",     "timber",   "timely",   "ting",     "tofu",     "together",
    "tolerate", "total",    "toxic",    "tracks",   "traffic",  "training",
    "transfer", "trash",    "traveler", "treat",    "trend",    "trial",
    "tricycle", "trip",     "triumph",  "trouble",  "true",     "trust",
    "twice",    "twin",     "type",     "typical",  "ugly",     "ultimate",
    "umbrella", "uncover",  "undergo",  "unfair",   "unfold",   "unhappy",
    "union",    "universe", "unkind",   "unknown",  "unusual",  "unwrap",
    "upgrade",  "upstairs", "username", "usher",    "usual",    "valid",
    "valuable", "vampire",  "vanish",   "various",  "vegan",    "velvet",
    "venture",  "verdict",  "verify",   "very",     "veteran",  "vexed",
    "victim",   "video",    "view",     "vintage",  "violence", "viral",
    "visitor",  "visual",   "vitamins", "vocal",    "voice",    "volume",
    "voter",    "voting",   "walnut",   "warmth",   "warn",     "watch",
    "wavy",     "wealthy",  "weapon",   "webcam",   "welcome",  "welfare",
    "western",  "width",    "wildlife", "window",   "wine",     "wireless",
    "wisdom",   "withdraw", "wits",     "wolf",     "woman",    "work",
    "worthy",   "wrap",     "wrist",    "writing",  "wrote",    "year",
    "yelp",     "yield",    "yoga",     "zero",
};

/**
 * This array contains number representations of SLIP-39 words.
 * These numbers are determined how the words were entered on a
 * T9 keyboard with the following layout:
 *     ab (1)   cd (2)  ef (3)
 *     ghij (4) klm (5) nopq (6)
 *     rs (7)   tuv (8) wxyz (9)
 *
 * Each word is uniquely defined by four buttons.
 */
static const struct {
  uint16_t sequence;
  uint16_t index;
} words_button_seq[WORDS_COUNT] = {
    {1212, 0},     // academic
    {1216, 7},     // adapt
    {1236, 8},     // adequate
    {1242, 1},     // acid
    {1248, 9},     // adjust
    {1254, 10},    // admit
    {1263, 2},     // acne
    {1267, 11},    // adorn
    {1268, 3},     // acquire
    {1276, 4},     // acrobat
    {1281, 13},    // advance
    {1284, 5},     // activity
    {1285, 12},    // adult
    {1286, 14},    // advocate
    {1287, 6},     // actress
    {1315, 67},    // beam
    {1317, 68},    // beard
    {1318, 69},    // beaver
    {1326, 70},    // become
    {1327, 71},    // bedroom
    {1341, 72},    // behavior
    {1346, 73},    // being
    {1354, 74},    // believe
    {1356, 75},    // belong
    {1363, 76},    // benefit
    {1371, 15},    // afraid
    {1378, 77},    // best
    {1396, 78},    // beyond
    {1414, 16},    // again
    {1417, 23},    // ajar
    {1423, 19},    // aide
    {1436, 17},    // agency
    {1453, 79},    // bike
    {1465, 80},    // biology
    {1472, 20},    // aircraft
    {1473, 18},    // agree
    {1474, 82},    // bishop
    {1475, 21},    // airline
    {1476, 22},    // airport
    {1478, 81},    // birthday
    {1512, 83},    // black
    {1514, 35},    // ambition
    {1516, 84},    // blanket
    {1517, 24},    // alarm
    {1518, 25},    // album
    {1519, 34},    // amazing
    {1526, 26},    // alcohol
    {1537, 85},    // blessing
    {1543, 27},    // alien
    {1545, 86},    // blimp
    {1546, 87},    // blind
    {1548, 28},    // alive
    {1564, 29},    // alpha
    {1568, 36},    // amount
    {1573, 30},    // already
    {1583, 88},    // blue
    {1585, 32},    // aluminum
    {1586, 31},    // alto
    {1587, 37},    // amuse
    {1591, 33},    // always
    {1615, 38},    // analysis
    {1617, 48},    // apart
    {1618, 39},    // anatomy
    {1623, 40},    // ancestor
    {1624, 41},    // ancient
    {1629, 89},    // body
    {1643, 42},    // angel
    {1645, 44},    // animal
    {1647, 43},    // angry
    {1658, 90},    // bolt
    {1674, 91},    // boring
    {1676, 92},    // born
    {1679, 45},    // answer
    {1681, 49},    // aquatic
    {1683, 46},    // antenna
    {1684, 93},    // both
    {1686, 94},    // boundary
    {1694, 47},    // anxiety
    {1712, 95},    // bracelet
    {1716, 96},    // branch
    {1718, 97},    // brave
    {1721, 50},    // arcade
    {1731, 98},    // breathe
    {1736, 51},    // arena
    {1743, 99},    // briefing
    {1748, 52},    // argue
    {1753, 53},    // armed
    {1763, 56},    // aspect
    {1765, 100},   // broken
    {1768, 101},   // brother
    {1769, 102},   // browser
    {1784, 54},    // artist
    {1789, 55},    // artwork
    {1824, 104},   // budget
    {1825, 103},   // bucket
    {1828, 57},    // auction
    {1837, 60},    // average
    {1841, 61},    // aviation
    {1845, 105},   // building
    {1848, 58},    // august
    {1851, 106},   // bulb
    {1854, 107},   // bulge
    {1856, 108},   // bumpy
    {1862, 109},   // bundle
    {1864, 62},    // avoid
    {1868, 59},    // aunt
    {1872, 110},   // burden
    {1876, 111},   // burning
    {1879, 112},   // busy
    {1893, 113},   // buyer
    {1917, 63},    // award
    {1919, 64},    // away
    {1947, 65},    // axis
    {1953, 66},    // axle
    {2143, 114},   // cage
    {2147, 185},   // daisy
    {2151, 186},   // damage
    {2152, 115},   // calcium
    {2153, 116},   // camera
    {2156, 117},   // campus
    {2161, 119},   // capacity
    {2162, 187},   // dance
    {2164, 120},   // capital
    {2168, 121},   // capture
    {2169, 118},   // canyon
    {2171, 122},   // carbon
    {2172, 123},   // cards
    {2173, 124},   // careful
    {2174, 125},   // cargo
    {2175, 188},   // darkness
    {2176, 126},   // carpet
    {2178, 127},   // carve
    {2181, 189},   // database
    {2183, 128},   // category
    {2184, 190},   // daughter
    {2187, 129},   // cause
    {2312, 191},   // deadline
    {2315, 192},   // deal
    {2317, 193},   // debris
    {2318, 194},   // debut
    {2323, 195},   // decent
    {2324, 196},   // decision
    {2325, 197},   // declare
    {2326, 198},   // decorate
    {2327, 199},   // decrease
    {2345, 130},   // ceiling
    {2351, 201},   // demand
    {2354, 200},   // deliver
    {2361, 204},   // depart
    {2363, 205},   // depend
    {2364, 206},   // depict
    {2365, 207},   // deploy
    {2367, 202},   // density
    {2368, 131},   // center
    {2369, 203},   // deny
    {2371, 132},   // ceramic
    {2372, 208},   // describe
    {2373, 209},   // desert
    {2374, 210},   // desire
    {2375, 211},   // desktop
    {2378, 212},   // destroy
    {2381, 213},   // detailed
    {2383, 214},   // detect
    {2384, 215},   // device
    {2386, 216},   // devote
    {2414, 217},   // diagnose
    {2415, 133},   // champion
    {2416, 134},   // change
    {2417, 135},   // charity
    {2428, 218},   // dictate
    {2432, 136},   // check
    {2435, 137},   // chemical
    {2437, 138},   // chest
    {2438, 219},   // diet
    {2439, 139},   // chew
    {2453, 220},   // dilemma
    {2454, 221},   // diminish
    {2463, 141},   // cinema
    {2464, 222},   // dining
    {2465, 223},   // diploma
    {2471, 224},   // disaster
    {2472, 225},   // discuss
    {2473, 226},   // disease
    {2474, 227},   // dish
    {2475, 228},   // dismiss
    {2476, 229},   // display
    {2478, 230},   // distance
    {2481, 140},   // chubby
    {2483, 231},   // dive
    {2484, 142},   // civil
    {2486, 232},   // divorce
    {2517, 143},   // class
    {2519, 144},   // clay
    {2531, 145},   // cleanup
    {2543, 146},   // client
    {2545, 147},   // climate
    {2546, 148},   // clinic
    {2562, 149},   // clock
    {2564, 150},   // clogs
    {2567, 151},   // closet
    {2568, 152},   // clothes
    {2581, 153},   // club
    {2587, 154},   // cluster
    {2615, 155},   // coal
    {2617, 156},   // coastal
    {2624, 157},   // coding
    {2628, 233},   // document
    {2651, 234},   // domain
    {2653, 235},   // domestic
    {2654, 236},   // dominant
    {2656, 159},   // company
    {2658, 158},   // column
    {2676, 160},   // corner
    {2678, 161},   // costume
    {2683, 164},   // cover
    {2684, 237},   // dough
    {2686, 162},   // counter
    {2687, 163},   // course
    {2691, 165},   // cowboy
    {2696, 238},   // downtown
    {2712, 166},   // cradle
    {2713, 167},   // craft
    {2714, 239},   // dragon
    {2715, 240},   // dramatic
    {2719, 168},   // crazy
    {2731, 241},   // dream
    {2732, 169},   // credit
    {2737, 242},   // dress
    {2742, 170},   // cricket
    {2743, 243},   // drift
    {2745, 171},   // criminal
    {2746, 244},   // drink
    {2747, 172},   // crisis
    {2748, 173},   // critical
    {2768, 245},   // drove
    {2769, 174},   // crowd
    {2782, 175},   // crucial
    {2784, 246},   // drug
    {2786, 176},   // crunch
    {2787, 177},   // crush
    {2793, 247},   // dryer
    {2797, 178},   // crystal
    {2814, 179},   // cubic
    {2825, 248},   // duckling
    {2853, 249},   // duke
    {2858, 180},   // cultural
    {2871, 250},   // duration
    {2874, 181},   // curious
    {2875, 182},   // curly
    {2878, 183},   // custody
    {2917, 251},   // dwarf
    {2954, 184},   // cylinder
    {2961, 252},   // dynamic
    {3124, 323},   // facility
    {3128, 324},   // fact
    {3145, 325},   // failure
    {3146, 326},   // faint
    {3153, 327},   // fake
    {3154, 329},   // family
    {3156, 330},   // famous
    {3157, 328},   // false
    {3162, 331},   // fancy
    {3164, 332},   // fangs
    {3168, 333},   // fantasy
    {3173, 255},   // easel
    {3175, 253},   // early
    {3178, 254},   // earth
    {3179, 256},   // easy
    {3181, 334},   // fatal
    {3184, 335},   // fatigue
    {3186, 336},   // favorite
    {3196, 337},   // fawn
    {3243, 260},   // edge
    {3246, 257},   // echo
    {3248, 261},   // editor
    {3254, 258},   // eclipse
    {3265, 259},   // ecology
    {3282, 262},   // educate
    {3413, 338},   // fiber
    {3428, 339},   // fiction
    {3458, 340},   // filter
    {3461, 341},   // finance
    {3462, 342},   // findings
    {3464, 343},   // finger
    {3472, 346},   // fiscal
    {3473, 344},   // firefly
    {3474, 347},   // fishing
    {3475, 345},   // firm
    {3484, 263},   // either
    {3486, 348},   // fitness
    {3514, 273},   // email
    {3515, 349},   // flame
    {3516, 264},   // elbow
    {3517, 350},   // flash
    {3518, 351},   // flavor
    {3523, 265},   // elder
    {3531, 352},   // flea
    {3532, 266},   // election
    {3534, 267},   // elegant
    {3535, 268},   // element
    {3536, 269},   // elephant
    {3537, 274},   // emerald
    {3538, 270},   // elevator
    {3539, 353},   // flexible
    {3546, 354},   // flip
    {3547, 275},   // emission
    {3548, 271},   // elite
    {3561, 355},   // float
    {3563, 276},   // emperor
    {3564, 277},   // emphasis
    {3565, 278},   // employer
    {3567, 356},   // floral
    {3568, 279},   // empty
    {3573, 272},   // else
    {3583, 357},   // fluff
    {3624, 280},   // ending
    {3625, 281},   // endless
    {3626, 282},   // endorse
    {3628, 358},   // focus
    {3635, 283},   // enemy
    {3636, 285},   // enforce
    {3637, 284},   // energy
    {3641, 286},   // engage
    {3642, 292},   // epidemic
    {3646, 287},   // enjoy
    {3647, 293},   // episode
    {3651, 288},   // enlarge
    {3671, 359},   // forbid
    {3672, 360},   // force
    {3673, 361},   // forecast
    {3674, 362},   // forget
    {3675, 363},   // formal
    {3678, 364},   // fortune
    {3679, 365},   // forward
    {3681, 294},   // equation
    {3683, 290},   // envelope
    {3684, 295},   // equip
    {3686, 366},   // founder
    {3687, 289},   // entrance
    {3689, 291},   // envy
    {3712, 367},   // fraction
    {3714, 368},   // fragment
    {3717, 296},   // eraser
    {3721, 298},   // escape
    {3736, 369},   // frequent
    {3737, 370},   // freshman
    {3741, 371},   // friar
    {3742, 372},   // fridge
    {3743, 373},   // friendly
    {3762, 297},   // erode
    {3767, 374},   // frost
    {3768, 375},   // froth
    {3769, 376},   // frozen
    {3781, 299},   // estate
    {3784, 300},   // estimate
    {3815, 301},   // evaluate
    {3836, 302},   // evening
    {3842, 303},   // evidence
    {3845, 304},   // evil
    {3853, 377},   // fumes
    {3862, 378},   // funding
    {3865, 305},   // evoke
    {3873, 380},   // fused
    {3875, 379},   // furl
    {3912, 306},   // exact
    {3915, 307},   // example
    {3923, 308},   // exceed
    {3924, 309},   // exchange
    {3925, 310},   // exclude
    {3928, 311},   // excuse
    {3931, 322},   // eyebrow
    {3932, 312},   // execute
    {3937, 313},   // exercise
    {3941, 314},   // exhaust
    {3961, 316},   // expand
    {3963, 317},   // expect
    {3965, 318},   // explain
    {3967, 319},   // express
    {3968, 315},   // exotic
    {3983, 320},   // extend
    {3987, 321},   // extra
    {4125, 483},   // jacket
    {4147, 420},   // hairy
    {4151, 381},   // galaxy
    {4153, 382},   // game
    {4157, 421},   // hamster
    {4162, 422},   // hand
    {4164, 423},   // hanger
    {4171, 383},   // garbage
    {4172, 384},   // garden
    {4175, 385},   // garlic
    {4176, 386},   // gasoline
    {4178, 424},   // harvest
    {4183, 425},   // have
    {4184, 387},   // gather
    {4186, 426},   // havoc
    {4191, 428},   // hazard
    {4195, 427},   // hawk
    {4231, 452},   // idea
    {4236, 453},   // identify
    {4253, 454},   // idle
    {4312, 429},   // headset
    {4315, 430},   // health
    {4317, 431},   // hearing
    {4318, 432},   // heat
    {4356, 433},   // helpful
    {4363, 388},   // general
    {4364, 389},   // genius
    {4365, 392},   // geology
    {4367, 390},   // genre
    {4368, 391},   // genuine
    {4371, 434},   // herald
    {4372, 435},   // herd
    {4374, 436},   // hesitate
    {4375, 484},   // jerky
    {4378, 393},   // gesture
    {4393, 485},   // jewelry
    {4512, 394},   // glad
    {4514, 455},   // image
    {4516, 395},   // glance
    {4517, 396},   // glasses
    {4536, 397},   // glen
    {4545, 398},   // glimpse
    {4561, 456},   // impact
    {4565, 457},   // imply
    {4567, 458},   // improve
    {4568, 459},   // impulse
    {4616, 437},   // hobo
    {4618, 399},   // goat
    {4623, 463},   // index
    {4624, 464},   // indicate
    {4625, 460},   // include
    {4626, 461},   // income
    {4627, 462},   // increase
    {4628, 465},   // industry
    {4631, 466},   // infant
    {4636, 467},   // inform
    {4643, 468},   // inherit
    {4646, 486},   // join
    {4648, 469},   // injury
    {4651, 470},   // inmate
    {4652, 400},   // golden
    {4653, 440},   // home
    {4654, 438},   // holiday
    {4659, 439},   // holy
    {4673, 471},   // insect
    {4674, 472},   // inside
    {4675, 441},   // hormone
    {4676, 442},   // hospital
    {4678, 473},   // install
    {4681, 476},   // invasion
    {4683, 474},   // intend
    {4684, 475},   // intimate
    {4686, 477},   // involve
    {4687, 443},   // hour
    {4712, 401},   // graduate
    {4716, 402},   // grant
    {4717, 403},   // grasp
    {4718, 404},   // gravity
    {4719, 405},   // gray
    {4731, 406},   // greatest
    {4743, 407},   // grief
    {4745, 408},   // grill
    {4746, 409},   // grin
    {4747, 478},   // iris
    {4751, 479},   // island
    {4762, 410},   // grocery
    {4765, 480},   // isolate
    {4767, 411},   // gross
    {4768, 412},   // group
    {4769, 413},   // grownup
    {4785, 414},   // grumpy
    {4817, 415},   // guard
    {4824, 487},   // judicial
    {4835, 481},   // item
    {4837, 416},   // guest
    {4842, 488},   // juice
    {4843, 444},   // huge
    {4845, 417},   // guilt
    {4848, 418},   // guitar
    {4851, 445},   // human
    {4854, 446},   // humidity
    {4856, 489},   // jump
    {4857, 419},   // gums
    {4862, 490},   // junction
    {4864, 491},   // junior
    {4865, 492},   // junk
    {4867, 482},   // ivory
    {4868, 447},   // hunting
    {4871, 448},   // husband
    {4874, 449},   // hush
    {4875, 450},   // husky
    {4878, 494},   // justice
    {4879, 493},   // jury
    {4917, 451},   // hybrid
    {5123, 502},   // laden
    {5124, 549},   // machine
    {5125, 503},   // ladle
    {5129, 504},   // ladybug
    {5141, 550},   // magazine
    {5142, 551},   // maiden
    {5145, 552},   // mailman
    {5146, 553},   // main
    {5147, 505},   // lair
    {5151, 556},   // mama
    {5153, 554},   // makeup
    {5154, 555},   // making
    {5156, 506},   // lamp
    {5161, 557},   // manager
    {5162, 558},   // mandate
    {5164, 507},   // language
    {5167, 559},   // mansion
    {5168, 560},   // manual
    {5171, 561},   // marathon
    {5172, 562},   // march
    {5173, 509},   // laser
    {5174, 508},   // large
    {5175, 563},   // market
    {5176, 565},   // mason
    {5178, 564},   // marvel
    {5183, 566},   // material
    {5184, 567},   // math
    {5186, 510},   // laundry
    {5194, 568},   // maximum
    {5196, 569},   // mayor
    {5197, 511},   // lawsuit
    {5312, 512},   // leader
    {5313, 513},   // leaf
    {5316, 570},   // meaning
    {5317, 514},   // learn
    {5318, 515},   // leaves
    {5321, 571},   // medal
    {5324, 572},   // medical
    {5328, 516},   // lecture
    {5341, 517},   // legal
    {5343, 518},   // legend
    {5347, 519},   // legs
    {5351, 573},   // member
    {5356, 574},   // memory
    {5362, 520},   // lend
    {5364, 521},   // length
    {5368, 575},   // mental
    {5372, 576},   // merchant
    {5374, 577},   // merit
    {5376, 495},   // kernel
    {5383, 522},   // level
    {5384, 578},   // method
    {5387, 579},   // metric
    {5391, 496},   // keyboard
    {5413, 523},   // liberty
    {5417, 524},   // library
    {5423, 525},   // license
    {5426, 497},   // kidney
    {5427, 580},   // midst
    {5438, 526},   // lift
    {5451, 528},   // lilac
    {5452, 581},   // mild
    {5453, 527},   // likely
    {5454, 582},   // military
    {5459, 529},   // lily
    {5462, 498},   // kind
    {5463, 583},   // mineral
    {5464, 584},   // minister
    {5467, 530},   // lips
    {5468, 531},   // liquid
    {5471, 585},   // miracle
    {5478, 532},   // listen
    {5482, 499},   // kitchen
    {5483, 533},   // literary
    {5484, 534},   // living
    {5491, 535},   // lizard
    {5493, 586},   // mixed
    {5498, 587},   // mixture
    {5613, 537},   // lobe
    {5614, 588},   // mobile
    {5616, 536},   // loan
    {5621, 538},   // location
    {5623, 589},   // modern
    {5624, 590},   // modify
    {5643, 500},   // knife
    {5647, 591},   // moisture
    {5648, 501},   // knit
    {5653, 592},   // moment
    {5674, 539},   // losing
    {5676, 593},   // morning
    {5678, 594},   // mortgage
    {5682, 540},   // loud
    {5683, 598},   // move
    {5684, 595},   // mother
    {5686, 596},   // mountain
    {5687, 597},   // mouse
    {5691, 541},   // loyalty
    {5824, 599},   // much
    {5825, 542},   // luck
    {5853, 600},   // mule
    {5858, 601},   // multiple
    {5861, 543},   // lunar
    {5862, 544},   // lunch
    {5864, 545},   // lungs
    {5872, 602},   // muscle
    {5873, 603},   // museum
    {5874, 604},   // music
    {5878, 605},   // mustang
    {5898, 546},   // luxury
    {5946, 547},   // lying
    {5974, 548},   // lyrics
    {6123, 636},   // paces
    {6124, 637},   // pacific
    {6125, 638},   // package
    {6137, 618},   // obesity
    {6141, 641},   // pajamas
    {6142, 639},   // paid
    {6143, 619},   // object
    {6145, 606},   // nail
    {6146, 640},   // painting
    {6161, 644},   // papa
    {6162, 642},   // pancake
    {6163, 645},   // paper
    {6168, 643},   // pants
    {6172, 646},   // parcel
    {6173, 620},   // observe
    {6174, 617},   // oasis
    {6175, 647},   // parking
    {6178, 648},   // party
    {6181, 621},   // obtain
    {6183, 649},   // patent
    {6184, 607},   // national
    {6187, 650},   // patrol
    {6195, 651},   // payment
    {6197, 652},   // payroll
    {6231, 622},   // ocean
    {6312, 653},   // peaceful
    {6316, 654},   // peanut
    {6317, 655},   // peasant
    {6321, 656},   // pecan
    {6325, 608},   // necklace
    {6341, 609},   // negative
    {6361, 657},   // penalty
    {6362, 658},   // pencil
    {6372, 659},   // percent
    {6373, 660},   // perfect
    {6375, 661},   // permit
    {6378, 610},   // nervous
    {6383, 623},   // often
    {6384, 662},   // petition
    {6389, 611},   // network
    {6397, 612},   // news
    {6416, 663},   // phantom
    {6417, 664},   // pharmacy
    {6425, 668},   // pickup
    {6428, 669},   // picture
    {6432, 670},   // piece
    {6453, 671},   // pile
    {6463, 673},   // pipeline
    {6465, 672},   // pink
    {6468, 665},   // photo
    {6471, 666},   // phrase
    {6478, 674},   // pistol
    {6482, 675},   // pitch
    {6497, 667},   // physics
    {6514, 676},   // plains
    {6516, 677},   // plan
    {6517, 678},   // plastic
    {6518, 679},   // platform
    {6519, 680},   // playoff
    {6531, 681},   // pleasure
    {6548, 625},   // omit
    {6568, 682},   // plot
    {6586, 683},   // plunge
    {6595, 624},   // olympic
    {6712, 684},   // practice
    {6714, 628},   // orbit
    {6715, 626},   // oral
    {6716, 627},   // orange
    {6719, 685},   // prayer
    {6723, 629},   // order
    {6724, 630},   // ordinary
    {6731, 686},   // preach
    {6732, 687},   // predator
    {6734, 688},   // pregnant
    {6735, 689},   // premium
    {6736, 690},   // prepare
    {6737, 691},   // presence
    {6738, 692},   // prevent
    {6741, 631},   // organize
    {6743, 693},   // priest
    {6745, 694},   // primary
    {6746, 695},   // priority
    {6747, 696},   // prisoner
    {6748, 697},   // privacy
    {6749, 698},   // prize
    {6761, 699},   // problem
    {6762, 700},   // process
    {6763, 701},   // profile
    {6764, 702},   // program
    {6765, 703},   // promise
    {6767, 704},   // prospect
    {6768, 705},   // provide
    {6786, 706},   // prune
    {6815, 707},   // public
    {6816, 716},   // quantity
    {6817, 717},   // quarter
    {6825, 613},   // nuclear
    {6836, 633},   // oven
    {6837, 634},   // overall
    {6842, 718},   // quick
    {6843, 719},   // quiet
    {6851, 614},   // numb
    {6853, 615},   // numerous
    {6856, 709},   // pumps
    {6857, 708},   // pulse
    {6861, 712},   // pupal
    {6862, 632},   // ounce
    {6864, 710},   // punish
    {6869, 711},   // puny
    {6872, 713},   // purchase
    {6876, 714},   // purple
    {6956, 616},   // nylon
    {6963, 635},   // owner
    {6984, 715},   // python
    {7121, 722},   // radar
    {7123, 720},   // race
    {7124, 721},   // racism
    {7125, 775},   // sack
    {7131, 776},   // safari
    {7145, 723},   // railroad
    {7146, 724},   // rainbow
    {7147, 725},   // raisin
    {7151, 777},   // salary
    {7156, 778},   // salon
    {7158, 779},   // salt
    {7162, 726},   // random
    {7164, 728},   // rapids
    {7165, 727},   // ranked
    {7176, 729},   // raspy
    {7183, 782},   // saver
    {7184, 780},   // satisfy
    {7186, 781},   // satoshi
    {7197, 783},   // says
    {7216, 784},   // scandal
    {7217, 785},   // scared
    {7218, 786},   // scatter
    {7236, 787},   // scene
    {7243, 789},   // science
    {7246, 788},   // scholar
    {7268, 790},   // scout
    {7271, 791},   // scramble
    {7273, 792},   // screw
    {7274, 793},   // script
    {7276, 794},   // scroll
    {7312, 730},   // reaction
    {7313, 795},   // seafood
    {7315, 731},   // realize
    {7316, 732},   // rebound
    {7317, 796},   // season
    {7318, 733},   // rebuild
    {7321, 734},   // recall
    {7323, 735},   // receiver
    {7326, 736},   // recover
    {7327, 797},   // secret
    {7328, 798},   // security
    {7343, 739},   // reject
    {7345, 799},   // segment
    {7347, 737},   // regret
    {7348, 738},   // regular
    {7351, 740},   // relate
    {7353, 741},   // remember
    {7354, 742},   // remind
    {7356, 743},   // remove
    {7361, 745},   // repair
    {7362, 744},   // render
    {7363, 746},   // repeat
    {7364, 800},   // senior
    {7365, 747},   // replace
    {7368, 748},   // require
    {7372, 749},   // rescue
    {7373, 750},   // research
    {7374, 751},   // resident
    {7376, 752},   // response
    {7378, 753},   // result
    {7381, 754},   // retailer
    {7383, 757},   // revenue
    {7384, 758},   // review
    {7386, 756},   // reunion
    {7387, 755},   // retreat
    {7391, 759},   // reward
    {7412, 801},   // shadow
    {7413, 802},   // shaft
    {7415, 803},   // shame
    {7416, 804},   // shaped
    {7417, 805},   // sharp
    {7423, 811},   // sidewalk
    {7424, 762},   // rich
    {7435, 806},   // shelter
    {7437, 807},   // sheriff
    {7453, 812},   // silent
    {7454, 814},   // similar
    {7456, 815},   // simple
    {7458, 813},   // silver
    {7464, 816},   // single
    {7467, 808},   // short
    {7468, 809},   // should
    {7474, 810},   // shrimp
    {7478, 817},   // sister
    {7481, 763},   // rival
    {7483, 764},   // river
    {7495, 760},   // rhyme
    {7498, 761},   // rhythm
    {7516, 820},   // slap
    {7517, 827},   // smart
    {7518, 821},   // slavery
    {7531, 828},   // smear
    {7532, 822},   // sled
    {7535, 829},   // smell
    {7542, 823},   // slice
    {7545, 824},   // slim
    {7546, 818},   // skin
    {7547, 830},   // smirk
    {7548, 831},   // smith
    {7565, 832},   // smoking
    {7569, 825},   // slow
    {7584, 833},   // smug
    {7586, 819},   // skunk
    {7587, 826},   // slush
    {7612, 843},   // space
    {7614, 765},   // robin
    {7615, 834},   // snake
    {7616, 835},   // snapshot
    {7617, 844},   // spark
    {7624, 837},   // society
    {7625, 766},   // rocky
    {7631, 845},   // speak
    {7632, 846},   // species
    {7635, 847},   // spelling
    {7636, 848},   // spend
    {7638, 838},   // software
    {7639, 849},   // spew
    {7642, 850},   // spider
    {7643, 836},   // sniff
    {7645, 851},   // spill
    {7646, 852},   // spine
    {7647, 853},   // spirit
    {7648, 854},   // spit
    {7651, 767},   // romantic
    {7652, 839},   // soldier
    {7656, 768},   // romp
    {7658, 840},   // solution
    {7671, 855},   // spray
    {7674, 856},   // sprinkle
    {7678, 769},   // roster
    {7681, 857},   // square
    {7683, 858},   // squeeze
    {7685, 841},   // soul
    {7686, 770},   // round
    {7687, 842},   // source
    {7691, 771},   // royal
    {7812, 859},   // stadium
    {7813, 860},   // staff
    {7814, 873},   // subject
    {7815, 874},   // submit
    {7816, 861},   // standard
    {7817, 862},   // starting
    {7818, 863},   // station
    {7819, 864},   // stay
    {7831, 865},   // steady
    {7836, 866},   // step
    {7841, 875},   // sugar
    {7842, 867},   // stick
    {7845, 868},   // stilt
    {7846, 772},   // ruin
    {7848, 876},   // suitable
    {7853, 773},   // ruler
    {7856, 774},   // rumor
    {7863, 878},   // superior
    {7865, 877},   // sunlight
    {7867, 869},   // story
    {7871, 870},   // strategy
    {7873, 879},   // surface
    {7874, 871},   // strike
    {7876, 880},   // surprise
    {7878, 881},   // survive
    {7895, 872},   // style
    {7931, 882},   // sweater
    {7945, 883},   // swimming
    {7946, 884},   // swing
    {7948, 885},   // switch
    {7951, 886},   // symbolic
    {7956, 887},   // sympathy
    {7962, 888},   // syndrome
    {7978, 889},   // system
    {8125, 890},   // tackle
    {8126, 892},   // tadpole
    {8128, 891},   // tactics
    {8153, 893},   // talent
    {8154, 965},   // valid
    {8156, 967},   // vampire
    {8158, 966},   // valuable
    {8164, 968},   // vanish
    {8174, 969},   // various
    {8175, 894},   // task
    {8178, 895},   // taste
    {8184, 896},   // taught
    {8194, 897},   // taxi
    {8312, 898},   // teacher
    {8315, 899},   // teammate
    {8317, 900},   // teaspoon
    {8341, 970},   // vegan
    {8356, 901},   // temple
    {8358, 971},   // velvet
    {8361, 902},   // tenant
    {8362, 903},   // tendency
    {8367, 904},   // tension
    {8368, 972},   // venture
    {8372, 973},   // verdict
    {8374, 974},   // verify
    {8375, 905},   // terminal
    {8378, 906},   // testify
    {8379, 975},   // very
    {8383, 976},   // veteran
    {8393, 977},   // vexed
    {8398, 907},   // texture
    {8416, 908},   // thank
    {8418, 909},   // that
    {8423, 979},   // video
    {8425, 917},   // ticket
    {8428, 978},   // victim
    {8429, 918},   // tidy
    {8431, 910},   // theater
    {8436, 911},   // theory
    {8437, 912},   // therapy
    {8439, 980},   // view
    {8451, 919},   // timber
    {8453, 920},   // timely
    {8459, 946},   // ugly
    {8464, 921},   // ting
    {8465, 982},   // violence
    {8467, 913},   // thorn
    {8468, 981},   // vintage
    {8471, 983},   // viral
    {8473, 914},   // threaten
    {8474, 984},   // visitor
    {8478, 985},   // visual
    {8481, 986},   // vitamins
    {8485, 915},   // thumb
    {8486, 916},   // thunder
    {8517, 948},   // umbrella
    {8584, 947},   // ultimate
    {8621, 987},   // vocal
    {8623, 950},   // undergo
    {8626, 949},   // uncover
    {8631, 951},   // unfair
    {8636, 952},   // unfold
    {8638, 922},   // tofu
    {8641, 953},   // unhappy
    {8642, 988},   // voice
    {8643, 923},   // together
    {8646, 954},   // union
    {8647, 960},   // upgrade
    {8648, 955},   // universe
    {8653, 924},   // tolerate
    {8654, 956},   // unkind
    {8656, 957},   // unknown
    {8658, 989},   // volume
    {8678, 961},   // upstairs
    {8681, 925},   // total
    {8683, 990},   // voter
    {8684, 991},   // voting
    {8687, 958},   // unusual
    {8694, 926},   // toxic
    {8697, 959},   // unwrap
    {8712, 927},   // tracks
    {8713, 928},   // traffic
    {8714, 929},   // training
    {8716, 930},   // transfer
    {8717, 931},   // trash
    {8718, 932},   // traveler
    {8731, 933},   // treat
    {8736, 934},   // trend
    {8737, 962},   // username
    {8741, 935},   // trial
    {8742, 936},   // tricycle
    {8743, 963},   // usher
    {8746, 937},   // trip
    {8748, 938},   // triumph
    {8768, 939},   // trouble
    {8781, 964},   // usual
    {8783, 940},   // true
    {8787, 941},   // trust
    {8942, 942},   // twice
    {8946, 943},   // twin
    {8963, 944},   // type
    {8964, 945},   // typical
    {9156, 992},   // walnut
    {9175, 993},   // warmth
    {9176, 994},   // warn
    {9182, 995},   // watch
    {9189, 996},   // wavy
    {9312, 999},   // webcam
    {9315, 997},   // wealthy
    {9316, 998},   // weapon
    {9317, 1019},  // year
    {9352, 1000},  // welcome
    {9353, 1001},  // welfare
    {9356, 1020},  // yelp
    {9376, 1023},  // zero
    {9378, 1002},  // western
    {9428, 1003},  // width
    {9435, 1021},  // yield
    {9452, 1004},  // wildlife
    {9462, 1005},  // window
    {9463, 1006},  // wine
    {9472, 1008},  // wisdom
    {9473, 1007},  // wireless
    {9484, 1009},  // withdraw
    {9487, 1010},  // wits
    {9641, 1022},  // yoga
    {9651, 1012},  // woman
    {9653, 1011},  // wolf
    {9675, 1013},  // work
    {9678, 1014},  // worthy
    {9716, 1015},  // wrap
    {9747, 1016},  // wrist
    {9748, 1017},  // writing
    {9768, 1018},  // wrote
};

#endif
