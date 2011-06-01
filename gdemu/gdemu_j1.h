/**
 * J1Class
 * $Id$
 * \file gdemu_j1.h
 * \brief J1Class
 * \date 2011-05-26 19:52GMT
 */

#ifndef GDEMU_J1_H
#define GDEMU_J1_H
// #include <...>

// System includes

// Project includes

namespace GDEMU {

/**
 * J1Class
 * \brief J1Class
 * \date 2011-05-26 19:52GMT
 */
class J1Class
{
public:
	J1Class() { }

	static void begin();
	//static void process();
	static void end();

	static void execute(int insnLimit, int microsLimit);
	static void flagReset(bool resetting);
	static bool isResetting();

	static long getCycles();

private:
	J1Class(const J1Class &);
	J1Class &operator=(const J1Class &);
	
}; /* class J1Class */

extern J1Class J1;

} /* namespace GDEMU */

#endif /* #ifndef GDEMU_J1_H */

/* end of file */
