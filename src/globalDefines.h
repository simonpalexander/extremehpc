/*
 * globalDefines.h
 *
 *  Created on: Apr 25, 2015
 *      Author: simon
 */

#ifndef SRC_GLOBALDEFINES_H_
#define SRC_GLOBALDEFINES_H_

// SUCCESS has value 0
// This is so it can be used in the OpenMPI solution, which expects a
// process to return a zero exit code.
enum { SUCCESS, FAIL };

enum { false, true };

#define successful(value) (value)? (false) : (true)

#endif /* SRC_GLOBALDEFINES_H_ */
