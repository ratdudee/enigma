#ifndef KEY_PARSER_H
#define KEY_PARSER_H

#include "config.h"



/* Parse a key from a key file. Supported simple text format (key=value lines):
   reflector=25
   rotors=3,1,4
   rings=A,B,C
   plugboard=AB,CD,EF
   Returns a newly allocated Key (caller must free with free_key) */
struct Key* parse_key_file(const char* path);

/* Parse a key from component strings (any may be NULL, but rotors and rings should match).
   Strings are the same format as above (comma-separated lists). */
struct Key* parse_key_components(const char* reflector_str, const char* rotors_str, const char* rings_str, const char* plugboard_str);

void free_key(struct Key* k);



#endif /* KEY_PARSER_H */
