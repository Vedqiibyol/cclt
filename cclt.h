#ifndef _CCLT
#define _CCLT

#include <types.h>
#include <defines.h>
#include <malloc.h>


#define CCLT_DEFAULT_RULE -1
#define CCLT_INVALID_RULE 0

uint __cclt_id = 1;
#define CCLT_ID __cclt_id++

// Note that for single char and parameters, it only matters that the first
// character is given, and only the first.
// this allows to do `-abcd` instead of `-a -b -c -d`, if `a` is set to grab
// more parameters, just use a placeholder pattern such as `#`
// The single_char behaviour is works, only if the pattern has a length of 1,
// and the amount is strictly superior to 0.
typedef struct {
	uint  id;
	char* pattern;
	uint  amount;
	bool  primary; // bypasses the pattern character rule
	bool  requiered;
	// bool track_individuals; // ??? I don't remember...
	// bool unique; // ???
	// u64  needed;
	// u64* needs;
	// // bool single_char; // -#abcd where # is the pattern and the rest are parameters
	// // ? Should this be always on? discared as a feature? oh, wait, nope, my bad. (2023-06-24)
	//   always one anyways
	// // bool use_equal; // the value in the result struct stands after an equal
	// // char separator; // is a character that separates the pattern from the value
	// // char separator; // default is 0
	bool separator; // does it use the separator given in the set
	bool capture_pool; // capture anything that comes after the specified amount
	// char* description; // used for the help
} cclt_rule;

typedef struct {
	cclt_rule* rule; // the rule the result has followed
	uint     amount; // the amount of parameters obtained
	// uint  pooled; // what was taken from the pool
	char**   values; // the parameters

	uint inherits; // primary rule this arguments belongs to
	// cclt_rule* inherits;

	cclt_rule* negated_by;
	cclt_result* negated_from;
	// {cclt_rule* by, cclt_result* from} negated;
} cclt_result;

typedef struct {
	// hint character is the character that preceeds a pattern,
	// in UNIX this character is '-' and '--' is used for words
	// in Windows this character is '/'
	char hint;
	// // bool double_hint; // UNIX behaviour for pattern with a length superior to 1

	// the separation character is the character that separates the argument
	// from a value usually it is `=`.
	// This behavior is ignored by single character patterns
	char sep;

	uint rule_count;
	cclt_rule* rules;

	uint result_count;
	cclt_result* results;

	uint  missing_count;
	cclt_rule** missings;

	// u64 negated_count;
	// u64 replaced_count;
	// u64 unfulfilled_count;

	// cclt_rule* negated;
	// cclt_rule* replaced;
	// cclt_rule* unfulfilled;

	u64 pooled;
	char** pool;

	char** argv;
	int argc;
} cclt_set;


cclt_set cclt_init() {
	cclt_set set = {};

	set.hint              = '-';
	set.double_hint       = true;

	set.rule_count        = 0;
	set.rules             = (cclt_rule*)malloc(32 * sizeof(cclt_rule));

	set.result_count      = 0;
	set.results           = nullptr;

	set.missing_count     = 0;
	set.missings          = nullptr;

	// set.negated_count     = 0;
	// set.replaced_count    = 0;
	// set.unfulfilled_count = 0;

	// set.negated           = nullptr;
	// set.replaced          = nullptr;
	// set.unfulfilled       = nullptr;

	set.argv              = nullptr;
	set.argc              = 0;

	return set;
}
cclt_rule cclt_create_rule(uint id, cc pattern, uint amount, bool requiered) {
	cclt_rule rule = {};

	rule.id        = id;
	rule.requiered = requiered;

	if (id != CCLT_DEFAULT_RULE && id != CCLT_INVALID_RULE) {
		rule.pattern = str_copy(pattern);
		rule.amount  = amount;
	} else {
		rule.pattern = nullptr;
		rule.amount  = 0;
	}

	return rule;
}
void _cclt_update(cclt_set* set) {
	if (set->hint == 0) set->hint = '-'; // printerr?
	if (set->sep  == 0) set->sep  = '=';

	set->results  = (cclt_result*)malloc(set->rule_count * sizeof(cclt_result));
	set->missings = (cclt_rule**)malloc(set->missing_count * sizeof(cclt_rule*));

	for (uint i=0, j=0; i < set->rule_count && j < set->missing_count; i++) {
		if (set->rules[i].requiered) {
			set->missings[j] = &set->rules[i];
			j++;
		}
	}
}
int _cclt_str_equal(cclt_set* set, cc str, cc pattern) {
	for (uint i=0; str[i] && pattern[i]; i++) {
		if (str[i] == set->sep)
			return i;
		if (str1[i] != str2[i])
			return -1;
	}

	return 0;
}

void cclt_add_rule(cclt_set* set, cclt_rule* rule) {
	set->rules[set->rule_count] = *rule;
	set->rule_count++;

	if (rule->requiered) set->missing_count++;
}



// Matches a given string with the patten of a rule
// - returns -1 if the string does not match
// - returns 0 if the string match
// - returns 1 on single character parameters
// - returns the position of the separator if any, not that it should be
//   superior to 2
uint _cclt_match_pattern(cclt_set* set, cc string, cclt_rule* rule) {
	// 1. does the rule wants to by pass its character rule (arg instead of --arg)
	// 2. check the string for errors (--a instead of -a)
	// 3. check rule if the pattern is only one character: (-a)
	// 4. check rule if wants multiple results for the single_char behaviour: (-a123)
	// 5. if the pattern has a length superior to 1, and excepts one or more arguments:

	// errors
	// if (cc == nullptr || string[0] == '\0') return false;

	if (string[0] != set->hint) {
		// ``prgm my_arg`` => true
		// ``prgm --arg``  => false
		if (rule->primary) {
			int r = _cclt_str_equal(set, string, pattern);
			return r;
			// - -1: not equal
			// - 0: equal
			// - anything other than 0 and -1: has a parameter, position of the
			//   separator is returned.
		}
		return -1;
		// string[0] == '-' or set's hint
	}

	// // 4.
	// // ! resulting should be dealt with by the parser
	// if (string[1] != set->hint) return 1;

	// 5.
	if (string[1] == set->hint) {
		uint r = _cclt_str_equal(set, string+2, pattern);
		return (r == -1) ? r : (rule->amount > 0) ? r + 2 : 0;
	}

	return -1;
}

uint _cclt_match_singlechar(cclt_set* set, cc string) {}

void cclt_parse(cclt_set* set, int argc, char** argv, bool update, u64 from) {
	if (update) { _cclt_update(set); }

	uint differcnt = 0;
	cclt_result* cur = &set->results[set->result_count];

	// ``set->rules[j].amount`` for the desired amount of parameters
	// ``cur->amount++;`` for the real amount of parameters parsed

	for (uint i=from; i < argc; i++, cur=&set->results[set->result_count]) {
		// * Saving
		// Puts the #parameters into the result struct
		if (differcnt) {
			cur->values[cur->amount++] = (char*)str_copy(argv[i]);

			differcnt--;
			if (!differcnt) set->result_count++;
			continue;
		}

		// * Arguments
		// ! REDEF
		// 1. First identify if the string starts with one or two hints
		//    - string[0] == set->hint
		//      1. It's important to note that if rule the string matches with
		//         requieres no other argument (`amount` < 1) multiple arguments
		//         can be checked but without any arguments
		//      2. If however the rule expects more that one argument, then
		//         potential value next aside the pattern should be tracked
		//    - string[1] == set->hint
		// 2. Check if the string matches any of the primary rules
		// 3. Find some way to save the arguments held in the string if any
		// 4. Ignore
		for (uint j=0; j < set->rule_count; j++) {
			// If the current argument follows a certain rule 
			// if (str_equal(argv[i], set->rules[j].pattern)) {
			// TODO: define some behavior			
			uint r = _cclt_match_pattern(set, argv[i], set->rules[j]);
			if (r != -1) {
				// set rule link-pointer
				cur->rule = &set->rules[j];
				cur->amount = 0;

				// creates a buffer for the index of each value
				// sets the first offset to 0
				if (set->rules[j].amount) {
					cur->values = (char**)malloc(set->rules[j].amount * sizeof(char*));
				} else { set->result_count++; }

				// the [parameters](#parameters) section will be executed if
				// this is set to something other than 0
				differcnt = set->rules[j].amount;

				// remove from the set->missings
				if (cur->rule->requiered) {
					for (uint i=0; i < set->missing_count; i++) {
						if (set->missings[i] == &cur->rules) {
							set->missings[j] = nullptr;
						}
					}
				}

				break;
			}
		}

		// * Pooling


		// * errors

	}
}

void cclt_destroy(cclt_set* set) {
	// destroy the results: deallocated [char**] values
	// deallocate the results saved
	// deallocate the rules of the set

	for (uint i=0; i < set->result_count; i++) {
		for (uint j=0; j < set->results[i].amount; j++)
			free(set->results[i].values[j]);
	}

	free(set->results);
	free(set->rules);

	// deallocate the list of missing options
	// deallocate the list of negated options
	// deallocate the list of replaced options
	// deallocate the list of unfulfilled options

	free(set->missings);
	// free(set->negated);
	// free(set->replaced);
	// free(set->unfilfilled);

	// - destroy the pool

}



void cclt_delete_rule(cclt_rule* rule) {
	free(rule->pattern);
}

// void cclt_print_rules(cclt_set*) {}
// void cclt_print_results(cclt_set*);

// u64  cclt_any_results(cclt_set*);

// u64  cclt_any_missing(cclt_set*);
// void cclt_get_missings(cclt_set*, u64*);
// void cclt_is_missing(cclt_set*, u64);

// u64  cclt_any_negated(cclt_set*);
// u64  cclt_any_replaced(cclt_set*);
// u64  cclt_any_unfulfilled(cclt_set*);

// void cclt_get_negated(cclt_set*, u64*);
// void cclt_get_replaced(cclt_set*, u64*);
// void cclt_get_unfulfilled(cclt_set*, u64*);

// void cclt_export(cclt_set*, cclt_set*, bool);
// void cclt_export_results(cclt_set*, cclt_results*, bool);



#endif

/*

void cclt_init(
	cclt_set* set
);

void cclt_add_rule(
	cclt_set*  set,
	cclt_rule* rule,
	bool       copy_rule
);
void cclt_parse(
	cclt_set* set,
	int       argc,
	char**    argv
);

void cclt_print_rules(
	cclt_set* set
);
void cclt_print_results(
	cclt_set* set
);

u64  cclt_any_results(
	cclt_set* set
);

u64  cclt_any_missing(
	cclt_set* set
);
void cclt_get_missigs(
	cclt_set* set,
	u64*      missing_ids_buffer
);
void cclt_is_missing(
	cclt_set* set,
	u64       rule_id
);

u64  cclt_any_negated(
	cclt_set* set
);
u64  cclt_any_replaced(
	cclt_set* set
);
u64  cclt_any_unfulfilled(
	cclt_set* set
);

void cclt_get_negated(
	cclt_set* set,
	u64*      negated_ids_buffer
);
void cclt_get_replaced(
	cclt_set* set,
	u64*      replaced_ids_buffer
);
void cclt_get_unfulfilled(
	cclt_set* set,
	u64*      unfulfilled_ids_buffer
);

void cclt_export(
	cclt_set* source_set,
	cclt_set* destination_set,
	bool      copy_args
);
void cclt_export_results(
	cclt_set*     source_set,
	cclt_results* desintation_results,
	bool          copy_args
);*/