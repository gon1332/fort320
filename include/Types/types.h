#ifndef __TYPES_H__
#define __TYPES_H__
/*
 * =====================================================================================
 *
 *       Filename:  types.h
 *
 *    Description:  Definition of types for FORT320 programming language
 *
 *        Version:  1.0
 *        Created:  07/15/2014 12:12:57 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Ioannis Konstadelias (gon1332), ikonstadel@gmail.com
 *        Company:  -
 *
 * =====================================================================================
 */


typedef enum {
	TY_invalid=0, TY_integer, TY_real, TY_character, TY_logical, TY_complex,
	TY_string, TY_unknown
} Type;				/* ----------  end of enum Type  ---------- */


typedef enum {
	C_variable=0, C_const, C_common, C_list, C_array, C_unknown
} Complex_Type;		/* ----------  end of enum Complex_Type  ---------- */

extern const char *typeNames[];

extern const char *catNames[];

#endif
