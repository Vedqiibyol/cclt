# CCLT - C command line toolkit

This tool kit should allow you to make parsing command line much easier, and
faster, with fewer lines, and less headaches.

The tool kit allows you to parse arguments using the Unix standard, with a
single letter flag with only one dash, a multiple letter flag with many,
using the equal operator, and more.

## Simple example

```C
#include <cclt.h>
#include <stdio.h>

int main(int argc, char** argv) {
    cclt_set set = cclt_init();

    cclt_rule help = cclt_create_rule(CCLT_ID, 'h', 0, false);
    cclt_rule argn = cclt_create_rule(CCLT_ID, 'num', 1, 1);

    cclt_add_rule(&set, &help, false);
    cclt_add_rule(&set, &argn, false);

    cclt_parse(&set, argc, argv);

    u64 missings = cclt_any_missing(&set);
    printf("%i\n", missings);

    cclt_print_results(&set);

    return 0;
}
```

# Concepts

1. Set
	The *set* is the object that holds *rules* and is recipient of all
	*results*. The behaviour that will ensue is defined by the *set*.
2. Hint
	A *hint* is a member of a *set*, it holds a character that is thereof seeked
	when given data. The hint identifies the argument.
3. Separator
	A *separator* is a member of a *set*, it holds a character that is thereof
	seeked in potential data. This *separator* is supposedly a splice of the
	*pattern* or arguments, and the value.
4. Rule
	A *rule* is an object that holds a *pattern* as well as an *ID* and a set
	of variable that define part of its behavior. When parsing, CCLT attemps to
	match given data to the defined behaviors in the given rules.
5. Primary rules
	A *primary rule* bypasses a certain behavior when parsed. The parser does
	not look if the arguments is *hinted*, and directly fulfills a *result*.
	Primary rules are also linked into the *results* of *non-primary* rule's
	result.
6. Result
	A *result* holds the *rule* it has followed and data it has gathered.
7. ID
	An *ID* is simply an integer that the developer can use to more easily
	refer to their rules.
8. Pattern
	A *pattern* is a string of text held by a *rule*, this pattern defines what
	the parser should be looking for when trying to obtain an argument.
9. Amount
	The *amount* is a property of a *rule* and tells the parser how many
	arguments or values are expected after a certain *pattern* got matched.
10. Merged pattern
	*Merged patterns* are single character-*patterns* that are bundles together.
11. Singleton
	A *singleton* defines a tuple of *pattern* or argument and its associated
	value, inside a single string, separated by a *separator*.
12. Pool
	A *pool* is a list of string where all non-matched arguments get put in. A
	*rule* can override to destination to its own *pool*.

# Progress

## Tasks
- [~] Change the ID system to a type system.
	-> Change is unecessary, the user may use the IDs to find his the results
	they are looking for
	-> furthermore, having same IDs can be use for the purpose of
	non-replaceable, replaceable and negated rules.
- [ ] Create a more sofisticated argument fetcher

## Functions
- [-] `cclt_init`
- [x] `cclt_create_rule`
	creates a new rule with a heap allocated pattern, must be destroyed with `cclt_delete_rule`.
- [x] `cclt_add_rule` adds a rule to a set
- [-] `cclt_parse` parse the inputs
	- [-] storing the values for each result in a ``char**``, not ``char*`` with
		``u64*``
- [-] `cclt_destroy`
- [-] `cclt_delete_rule`
- [ ] `cclt_print_rules`
- [ ] `cclt_print_results`
- [ ] `cclt_any_results`
- [ ] `cclt_any_missing`
- [ ] `cclt_get_missings`
- [ ] `cclt_is_missing`
- [ ] `cclt_any_negated`
- [ ] `cclt_any_replaced`
- [ ] `cclt_any_unfulfilled`
- [ ] `cclt_get_negated`
- [ ] `cclt_get_replaced`
- [ ] `cclt_get_unfulfilled`
- [ ] `cclt_export`
- [ ] `cclt_export_results`

## Library's functions
These function should not be used by the user of the library.
- [ ] `_cclt_update`
- [ ] `_cclt_str_equal`
- [ ] `_cclt_match_pattern`
- [ ] `_cclt_match_singlechar`



