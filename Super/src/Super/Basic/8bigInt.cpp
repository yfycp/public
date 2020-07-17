// ==============================================================
//
//  Copyright (C) 1995  William A. Rossi
//                      class RossiBigInt
// 
//  Copyright (C) 1999-2016  Alex Vinokur
//                           class BigInt 
//                           class BigInt::BaseBigInt 
//                           class BigInt::Vin
//                           upgrading class BigInt::Rossi
//                           class BigInt::Run
//                           class BigInt::Test
//                           class BigInt::TestVin
//                           class BigInt::TestRossi
//                           class BigInt::PerformanceTestVin
//                           class BigInt::PerformanceTestRossi 
//
//  ------------------------------------------------------------
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//  ------------------------------------------------------------
// 
//  mailto:alex DOT vinokur AT gmail DOT com
//  http://sourceforge.net/users/alexvn/
//
// ==============================================================


// ##############################################################
//
//  SOFTWARE : Class BigInt
//  FILE     : bigInt.cpp
//
//  DESCRIPTION : Implementation of the classes
//								* BigInt 
//								* BigInt::BaseBigInt 
//								* BigInt::Vin
//								* BigInt::Rossi
//								* BigInt::Run
//								* BigInt::Test
//								* BigInt::TestVin
//								* BigInt::TestRossi
//
// ##############################################################

// include section
#include "8bigInt.h"

#if (defined unix || defined __unix || defined __unix__) 
#include <unistd.h>
#include <sys/utsname.h>
#endif


// =================
// Constants
// =================
static const BigInt::Rossi RossiZero (0);
static const BigInt::Rossi RossiOne  (1);
static const BigInt::Rossi RossiTwo  (2);
static const BigInt::Rossi RossiThree (3);
// static const BigInt::Rossi RossiTen  = BigInt::Rossi(BigInt::toString(std::numeric_limits<BigInt::Unit>::max()), BigInt::DEC_DIGIT) + RossiOne;

static const BigInt::Vin VinZero (0);
static const BigInt::Vin VinOne  (1);

const std::string BigInt::s_strHelp("help");
const std::string BigInt::s_strDemo("demo");
const std::string BigInt::s_strTest("test");
const std::string BigInt::s_strTime("time");


// =================
// Statics
// =================
std::size_t  BigInt::Test::s_returnValue = 0;
std::clock_t BigInt::Test::s_startTime   = 0;
std::clock_t BigInt::Test::s_endTime     = 0;
std::clock_t BigInt::Test::s_elapsedTime = 0;

std::ostringstream BigInt::Test::s_oss;	

const std::clock_t BigInt::Test::s_clockMinThreshold = 1 * CLOCKS_PER_SEC; 



// ------------------------
static const std::pair<char, std::size_t> s_hex2dec[] =
{
    std::make_pair ('0', 0),
    std::make_pair ('1', 1),
    std::make_pair ('2', 2),
    std::make_pair ('3', 3),
    std::make_pair ('4', 4),
    std::make_pair ('5', 5),
    std::make_pair ('6', 6),
    std::make_pair ('7', 7),
    std::make_pair ('8', 8),
    std::make_pair ('9', 9),
    std::make_pair ('a', 10),
    std::make_pair ('A', 10),
    std::make_pair ('b', 11),
    std::make_pair ('B', 11),
    std::make_pair ('c', 12),
    std::make_pair ('C', 12),
    std::make_pair ('d', 13),
    std::make_pair ('D', 13),
    std::make_pair ('e', 14),
    std::make_pair ('E', 14),
    std::make_pair ('f', 15),
    std::make_pair ('F', 15)
};

std::map<char, std::size_t> BigInt::BaseBigInt::ms_hex2dec (array2map(s_hex2dec));

// =================
// Macros
// =================

#define BIGINT_TEST_COMPUTE_BINARY_OP(outstream, type, args, op) \
	  type z = args.first op args.second; \
	  outstream << "[Hex] " \
                << args.first \
	            << " " \
	            << #op \
	            << " " \
                << args.second \
	            << " = " \
	            << z \
	            << std::endl; \
	  outstream << "[Dec] " \
				<< args.first.toStrDec() \
	            << " " \
	            << #op \
	            << " " \
                << args.second.toStrDec() \
	            << " = " \
	            << z.toStrDec() \
				<< std::endl \
	            << std::endl

#define BIGINT_TEST_COMPUTE_BINARY_UNIT_OP(outstream, type, args, op) \
	  type z = args.first op args.second; \
	  outstream << "[Hex] " \
                << args.first \
	            << " " \
	            << #op \
	            << " " \
                << std::hex \
                << std::showbase \
                << args.second \
                << std::dec \
	            << " = " \
	            << z \
	            << std::endl; \
	outstream << "[Dec] " \
                << args.first.toStrDec() \
	            << " " \
	            << #op \
	            << " " \
                << args.second \
	            << " = " \
	            << z.toStrDec() \
	            << std::endl \
				<< std::endl
	

#define BIGINT_TEST_COMPARE_BINARY_OP(outstream, args, op) \
	  const bool z = args.first op args.second; \
	  outstream << "[Hex] " \
				<< args.first \
	            << " " \
	            << #op \
	            << " " \
                << args.second \
	            << " = " \
	            << (z ? "TRUE" : "FALSE") \
	            << std::endl; \
	  outstream << "[Dec] " \
				<< args.first.toStrDec() \
	            << " " \
	            << #op \
	            << " " \
                << args.second.toStrDec() \
	            << " = " \
	            << (z ? "TRUE" : "FALSE") \
	            << std::endl \
				<< std::endl

#define BIGINT_TEST_COMPUTE_UNARY_OP(outstream, args, op) \
	  { \
		  std::ostringstream ossFirstHex; \
		  ossFirstHex	<< args.first; \
		  std::ostringstream ossFirstDec;\
		  ossFirstDec << args.first.toStrDec(); \
		  args.first op args.second; \
		  outstream << "[Hex] " \
		            << ossFirstHex.str() \
					<< " " \
					<< #op \
					<< " " \
					<< args.second \
					<< " = " \
					<< args.first \
					<< std::endl; \
		  outstream << "[Dec] " \
		            << ossFirstDec.str() \
					<< " " \
					<< #op \
					<< " " \
					<< args.second.toStrDec() \
					<< " = " \
					<< args.first.toStrDec() \
					<< std::endl \
					<< std::endl; \
	 }


#define ROSSI_TEST_COMPUTE_BINARY_OP(outstream, args, op)         BIGINT_TEST_COMPUTE_BINARY_OP(outstream, BigInt::Rossi, args, op)
#define ROSSI_TEST_COMPUTE_BINARY_UNIT_OP(outstream, args, op)    BIGINT_TEST_COMPUTE_BINARY_UNIT_OP(outstream, BigInt::Rossi, args, op)

#define VIN_TEST_COMPUTE_BINARY_OP(outstream, args, op)           BIGINT_TEST_COMPUTE_BINARY_OP(outstream, BigInt::Vin, args, op)
#define VIN_TEST_COMPUTE_BINARY_UNIT_OP(outstream, args, op)      BIGINT_TEST_COMPUTE_BINARY_UNIT_OP(outstream, BigInt::Vin, args, op)



////////////
// FUNCTIONS
////////////
// ===============================
BigInt::Unit BigInt::get_VIN_BASE1 (const BigInt::Unit i_unitMax)
{
	Unit reminder = i_unitMax;
    Unit base1 = 0;
	while (true)
	{
		if (reminder == 0)
		{
			break;
		}

		reminder /= 10;
		base1++;
	}
	return base1;
}

// ===============================
BigInt::Unit BigInt::get_VIN_BASE2 (const BigInt::Unit i_base1)
{
	Unit base2 = 1;
	for (std::size_t i = 0; i < (static_cast<std::size_t>(i_base1) - 1); i++)
	{
			base2 *= 10;
	}
	return base2;
}

// ===============================
void BigInt::Run::showHelp(std::ostream& o_stream, const std::vector<std::string>& i_args)
{
    std::ostringstream oss;

	BigInt::showVersion(oss);
	BigInt::showSystem(oss);
	BigInt::showCompiler(oss);
	BigInt::showEnvironment(oss);
	BigInt::showCompilationOptions(oss);
	
	if (!BigInt::assertCheck(true))
	{
		std::cerr << oss.str() << std::endl;
	}
	BigInt::assertCheck();

	const std::string strExeFileName ("<exeFileName>");

    const std::string actualExeFileName (i_args.empty() ? strExeFileName : i_args[0]);

    oss << ""
        << std::endl

        << "USAGE : " 
		<< std::endl

		<< "      : " 
        << strExeFileName 
        << std::endl
		
		<< "      : " 
        << strExeFileName
		<< " "
		<< BigInt::s_strHelp
        << std::endl

		<< "      : " 
        << strExeFileName
		<< " "
		<< BigInt::s_strDemo
        << std::endl

		<< "      : " 
        << strExeFileName
		<< " "
		<< BigInt::s_strTest
        << std::endl

		<< "      : " 
        << strExeFileName
		<< " "
		<< BigInt::s_strTime
        << std::endl

        << "      : " 
        << strExeFileName 
        << " <first-arg-in-decimal> <binary-operation> <second-arg-in-decimal>" 
        << std::endl

        << std::endl;

    oss << ""
        << std::endl

        << "USAGE SAMPLE: " 
		<< std::endl
		
		<< "      : " 
        << actualExeFileName 
        << std::endl
        
		<< "      : " 
        << actualExeFileName
		<< " "
		<< BigInt::s_strHelp
        << std::endl

		<< "      : " 
        << actualExeFileName
		<< " "
		<< BigInt::s_strDemo
        << std::endl

		<< "      : " 
        << actualExeFileName
		<< " "
		<< BigInt::s_strTest
        << std::endl

		<< "      : " 
        << actualExeFileName
		<< " "
		<< BigInt::s_strTime
        << std::endl

        << "      : " 
        << actualExeFileName 
        << " 1123581321345589 + 123456789" 
        << std::endl

        << std::endl;

    o_stream << oss.str();
}

// ===============================
void BigInt::Run::showCommandLine(std::ostream& o_stream, int argc, char** argv)
{
  std::ostringstream oss;

	oss << std::endl;
	oss << std::endl;
	oss << "\t---> YOUR COMMAND LINE : ";
	for (int i = 0; i < argc; i++)
	{
		oss << argv[i] << " ";
	}
	oss << std::endl;
	oss << std::endl;

    o_stream << oss.str();
}


// ===============================
void BigInt::Run::showTime(std::ostream& o_stream, const std::string& i_msg)
{
  std::ostringstream oss;

	oss << std::endl;
	oss << std::endl;

	const time_t timer = time(NULL);

	

	oss << ""
		<< "\t";
	if (!i_msg.empty())
	{
		oss << ""
			<< i_msg
			<< ": ";
	}

	oss	<< std::asctime (std::localtime(&timer));

	oss << std::endl;
	oss << std::endl;

    o_stream << oss.str();
}





// ===============================
void BigInt::Run::showHelp(std::ostream& o_stream, const std::string& i_exeFileName)
{
	std::ostringstream oss;

	std::vector<std::string> args;

	if (!i_exeFileName.empty())
	{
		args.push_back(i_exeFileName);
	}

	showHelp(o_stream, args);

	o_stream << oss.str();
}


// ===============================
std::map<std::string, std::string> BigInt::Run::getSampleAllowedOperation()
{
    static std::map<std::string, std::string> allowedOperations;

    if (allowedOperations.empty())
    {
        allowedOperations["+"] = "Addition";
        allowedOperations["-"] = "Subtraction";
        allowedOperations["x"] = "Multiplication";
        allowedOperations["/"] = "Division";
        allowedOperations["%"] = "Reminder";
    }

    return allowedOperations;

   
}


// ===============================
bool BigInt::Run::checkCommandLine(std::ostream& o_stream, const std::vector<std::string>& i_args)
{
    const std::map<std::string, std::string> allowedOperations = getSampleAllowedOperation();

    switch (i_args.size())
    {
        case 0:
        case 1:
            showHelp (o_stream, i_args);
            return true;
            break;  // unused

        case 4:
            if (!allowedOperations.count (i_args[2]))
            {
                std::ostringstream oss;
                oss << ""
                    << "Illegal operation in argv[2] : " 
                    << i_args[2] 
                    << std::endl;
                oss << "Allowed operations are as follows: ";
                for (std::map<std::string, std::string>::const_iterator posIter = allowedOperations.begin();  
                     posIter != allowedOperations.end(); 
                     posIter++
                    )
                {
                    oss << posIter->first 
                        << " ";  
                }
                oss << std::endl
                    << std::endl;
                o_stream << oss.str();
                return false;
            }
            return true;
            break;  // unused

        default:
            showHelp (o_stream, i_args);
            return false;
            break;  // unused
    }
}

// ===============================
int BigInt::Run::mainBigInt(int argc, char** argv)
{
    std::vector<std::string> i_args;

    for (int i = 0; i < argc; i++)
    {
        i_args.push_back(argv[i]);
    }

    const int result = mainBigInt(std::cout, i_args);

    return result;
}

// ===============================
int BigInt::Run::mainBigInt(std::ostream& o_stream, const std::vector<std::string>& i_args)
{

	std::ostringstream oss;

    BigInt::showVersion(oss);
	BigInt::showSystem(oss);
	BigInt::showCompiler(oss);
	BigInt::showEnvironment(oss);
	BigInt::showCompilationOptions(oss);
	
	if (!BigInt::assertCheck(true))
	{
		std::cerr << oss.str() << std::endl;
	}
    BigInt::assertCheck();

	o_stream << oss.str();
  
    if ((i_args.size() == 2) && (i_args[1] == s_strTest))
    {
		applicationSimpleSample(o_stream);

        int retValue = 0;
#ifndef NO_TRY_CATCH
        try
        {
#endif // NO_TRY_CATCH
			BigInt::TestVin::testAssertAll(o_stream);
            BigInt::TestVin::testAll(o_stream);
			BigInt::TestRossi::testAssertAll(o_stream);
            BigInt::TestRossi::testAll(o_stream);
            BigInt::TestRossi::testTryCatch(o_stream);     
#ifndef NO_TRY_CATCH
        }
        catch (std::exception& i_e)
        {
            std::ostringstream oss;
            std::ostringstream ossFatal;
            ossFatal    << ""
                        << "EXCEPTION ===> "
                        << i_e.what() 
                        << std::endl;
            FATAL_MSG (oss, ossFatal.str());

            o_stream << oss.str();
          
            retValue = 1;
        }
        catch(...)
        {
            std::ostringstream oss;
            std::ostringstream ossFatal;
            ossFatal    << ""
                        << "UNKNOWN EXCEPTION"
                        << std::endl;
            FATAL_MSG (oss, ossFatal.str());

            o_stream << oss.str();
          
            retValue = 1;
        }
#endif // NO_TRY_CATCH
        showHelp (o_stream, i_args);
        return retValue;
    }


	if ((i_args.size() == 2) && (i_args[1] == s_strTime))
    {
        int retValue = 0;
#ifndef NO_TRY_CATCH
        try
        {
#endif // NO_TRY_CATCH
            BigInt::PerformanceTestVin::performanceTestAll(o_stream);
            BigInt::PerformanceTestRossi::performanceTestAll(o_stream);
#ifndef NO_TRY_CATCH
        }
        catch (std::exception& i_e)
        {
            std::ostringstream oss;
            std::ostringstream ossFatal;
            ossFatal    << ""
                        << "EXCEPTION ===> "
                        << i_e.what() 
                        << std::endl;
            FATAL_MSG (oss, ossFatal.str());

            o_stream << oss.str();
          
            retValue = 1;
        }
        catch(...)
        {
            std::ostringstream oss;
            std::ostringstream ossFatal;
            ossFatal    << ""
                        << "UNKNOWN EXCEPTION"
                        << std::endl;
            FATAL_MSG (oss, ossFatal.str());

            o_stream << oss.str();
          
            retValue = 1;
        }
#endif // NO_TRY_CATCH
        // showHelp (o_stream, i_args);
        return retValue;
    }




	if (!checkCommandLine (o_stream, i_args)) 
    {
        return 1;
    }

    // ----------------------
    ASSERTION (i_args.size() == 4);

	applicationSimpleSample(o_stream);
    runRossiInteractiveSample(o_stream, i_args);

    return 0;
}

// ==================================
void BigInt::showVersion(std::ostream& o_stream)
{
    std::ostringstream oss;

	const bool sizeofsAreCorrect = (sizeof(std::size_t) <= sizeof(Unit));

    oss << std::endl

		<< "BigInt - C++ class for computation with arbitrary large integers" 
		<< std::endl

		<< "---> Version " 
		<< BIG_INT_MAJOR_VERSION 
		<< "." 
		<< BIG_INT_MINOR_VERSION 
		<< std::endl

		<< "Project Home Page: "
		<< "https://sourceforge.net/projects/cpp-bigint/" 
        << std::endl

		<< std::endl

		<< "\tCompiled in " 
		<< (sizeof(void*) * CHAR_BIT) 
		<< "-bit mode" 
		<< std::endl

		<< "===> \tUnit size           : " 
		<< (sizeof(BigInt::Unit) * CHAR_BIT) 
		<< " bits" 
		<< std::endl

		<< "\tunsigned long size  : " 
		<< (sizeof(unsigned long) * CHAR_BIT) 
		<< " bits" 
		<< std::endl

		<< "\tstd::size_t size    : " 
		<< (sizeof(std::size_t) * CHAR_BIT) 
		<< " bits" 
		<< std::endl;

	if (!sizeofsAreCorrect)
	{
		/*
		oss << ""
			<< std::endl

			<< "ATTENTION!!! "
			<< "sizeof(std::size_t) and sizeof(Unit) are inconsistent:"
			<< std::endl

			<< "\t"
			<< "sizeof(std::size_t) = "
			<< sizeof(std::size_t)
			<< ", "
			<< "sizeof(Unit) = "
			<< sizeof(Unit)
			<< std::endl

			<< "\t"
			<< "It should be: sizeof(std::size_t) <= sizeof(Unit)"
			<< std::endl;
			*/
	}

	oss	<< std::endl

		<< "Copyright (C) 1995       William A. Rossi - class BigInt::Rossi" 
		<< std::endl
		<< "Copyright (C) 1999-2016  Alex Vinokur - class BigInt, class BigInt::BaseBigInt, class BigInt::Vin, upgrading class BigInt::Rossi, class BigInt::Run, class BigInt::Test, class BigInt::TestVin, class BigInt::TestRossi, class BigInt::PerformanceTestVin, class BigInt::PerformanceTestRossi"
		<< std::endl

		<< std::endl

		<< "This is free software; see the source for copying conditions." 
		<< std::endl
		<< "There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE." 
		<< std::endl

		<< std::endl;

    o_stream << std::flush << oss.str() << std::flush;
}


// ==================================
void BigInt::showSystem(std::ostream& o_stream)
{
    std::ostringstream oss;
    oss << std::endl;
 
#if (defined unix || defined __unix || defined __unix__) 
struct utsname inst_utsname;
  
	oss << "System Info:"
	    << std::endl;
  const int result = uname (&inst_utsname);
  if (result == 0)
  {
	  oss << ""

		  << "\t"
		  << "System Name : " 
		  << inst_utsname.sysname
		  << std::endl

		  << "\t"
		  << "Node Name   : " 
		  << inst_utsname.nodename
		  << std::endl

		  << "\t"
		  << "Release     : " 
		  << inst_utsname.release
		  << std::endl

		  << "\t"
		  << "Version     : " 
		  << inst_utsname.version
		  << std::endl

		  << "\t"
		  << "Machine     : " 
		  << inst_utsname.machine
		  << std::endl;
  }
  else
  {
	  oss << ""
	      << "\t"
	      << "Unable to get system/machine info"
		  << std::endl;
  }
#endif  

    o_stream << std::flush << oss.str() << std::flush;
}



// ==================================
void BigInt::showCompilationOptions(std::ostream& o_stream)
{
    std::ostringstream oss;

    oss << std::endl;

	oss << "Compilation Options:"
	    << std::endl;

	oss << "\tUNIT32       : ";
#ifdef UNIT32
	oss << "defined";
#else 
	oss << "undefined (Default)";
#endif	
	oss	<< std::endl;

	oss << "\tNO_TRY_CATCH : ";
#ifdef NO_TRY_CATCH
	oss << "defined";
#else 
	oss << "undefined (Default)";
#endif	
	oss	<< std::endl;

	oss << std::endl;

    o_stream << std::flush << oss.str() << std::flush;
}


// ==================================
void BigInt::showEnvironment(std::ostream& o_stream)
{
    std::ostringstream oss;

	#define DISPLAY_SETTING(s) oss << "\t" << #s << " \t: " << std::hex << std::showbase << s << " " << std::dec << s << std::endl


    oss << std::endl;

	oss << "Program Environment:"
	    << std::endl;

	oss << "\tData Model : "
		<< (sizeof(void*) * CHAR_BIT)
		<< "-bit"
		<< std::endl;

	oss << "\tUNIT64     : ";
#ifdef UNIT64
	oss << "defined (Default)";
#else
	oss << "undefined";
#endif
	oss << std::endl;


	oss << "\tUNIT32     : ";
#ifdef UNIT32
	oss << "defined";
#else
	oss << "undefined (Default)";
#endif
	oss << std::endl;

	oss << "\tUnit size  : " 
		<< (sizeof(BigInt::Unit) * CHAR_BIT) 
		<< " bits" 
		<< std::endl;

	oss << "\tsize_t size: " 
		<< (sizeof(std::size_t) * CHAR_BIT) 
		<< " bits" 
		<< std::endl;
	
	oss << std::endl;

	DISPLAY_SETTING(ULONG_MAX);
	DISPLAY_SETTING(UNIT_MAX);
	DISPLAY_SETTING(UNIT_MSB);
	DISPLAY_SETTING(VIN_UNIT_MAX);
	DISPLAY_SETTING(VIN_BASE1);
	DISPLAY_SETTING(VIN_BASE2);
	DISPLAY_SETTING(VIN_SUB_BASE2);
	DISPLAY_SETTING(VIN_OVER_BASE2);
  
	oss << std::endl;
  
 
#undef DISPLAY_SETTING

    o_stream << std::flush << oss.str() << std::flush;
}






// ==================================
void BigInt::showCompiler(std::ostream& o_stream)
{
	// -----------------------------
	// Pre-defined Compiler Macros
	// https://sourceforge.net/p/predef/wiki/Compilers
	// -----------------------------

    std::ostringstream oss;
	std::ostringstream ossInfo;
	std::ostringstream ossOptimization;

    oss << std::endl;
 
	oss << "Compiler Info:"
	    << std::endl;

// ------ GNU gcc ------
#if (defined __GNUC__ && !defined __INTEL_COMPILER)
	std::string gcc_or_gpp;
#ifdef __GNUG__
	gcc_or_gpp = "g++";
#else
	gcc_or_gpp = "gcc";
#endif
	ossInfo << ""
	        << "\t"
		    << "GNU "
			<< gcc_or_gpp
		    << " version " 
			<< __GNUC__;

#ifdef __GNUC_MINOR__  
    ossInfo << "." 
		    << __GNUC_MINOR__;
#ifdef __GNUC_PATCHLEVEL__   
#if (__GNUC_PATCHLEVEL__)
    ossInfo << "." 
		    << __GNUC_PATCHLEVEL__;
#endif // if __GNUC_PATCHLEVEL__
#endif // ifdef __GNUC_PATCHLEVEL__
#endif // __GNUC_MINOR__


	ossOptimization << ""
		            << "\t---> Compiled ";
#ifdef __OPTIMIZE__
	ossOptimization << "with";
#else
	ossOptimization << "without";
#endif
	ossOptimization << " optimization";

#endif // __GNUC__


// ------ Intel compiler ------
#if (defined __INTEL_COMPILER)
	ossInfo << ""
	        << "\t"
			<< "Intel C++ compiler: Version "
			<< __INTEL_COMPILER 
			<< ", Build Date: "
			<< __INTEL_COMPILER_BUILD_DATE;

#endif


// ------ HP compiler ------
#if (defined __HP_aCC)
	ossInfo << ""
	        << "\t"
			<< "HP aC++ compiler: Version "
			<< __HP_aCC; 
#endif


// ------ IBM XL C/C++ compiler ------
#if (defined __xlC__)
	ossInfo << ""
	        << "\t"
			<< "IBM XL C/C++ compiler: Version "
			<< __xlC__; 
#endif



// ------ Sun Solaris C++ compiler ------
#if (defined __SUNPRO_CC)
	ossInfo << ""
	        << "\t"
			<< "Sun Solaris C++ compiler: Version "
			<< __SUNPRO_CC; 
#endif


// ------ Microsoft C++ ------
#ifdef _MSC_VER
	ossInfo << ""
	        << "\t"
			<< "Microsoft C++ Compiler Version "
			<< _MSC_VER
			<< ", Full Version "
			<< _MSC_FULL_VER;

#endif // _MSC_VER

	oss << ""
		<< "\t"
		<< "C++ version: ";

	if (__cplusplus >= 201100L)
	{
		oss << "C++11 and higher";
	}
	else
	{
		oss << "lower than C++11";
	}
	oss << std::endl;

    if (!ossInfo.str().empty())
	{
		oss << ossInfo.str()
			<< std::endl
			<< ossOptimization.str()
			<< std::endl;

		
	}
	else
	{
		oss << std::endl;
		oss << ""
			<< "\t"
			<< "Unknown compiler"
			<< std::endl
			
			<< "\t"
			<< "You can add info of your compiler in function "
			<< std::endl

			<< "\t"
			<< FUNCTION_NAME
			<< std::endl

			<< "\t"
			<< "File: "
			<< __FILE__
			<< ", Line#"
			<< __LINE__
			<< std::endl;
	}

    o_stream << std::flush << oss.str() << std::flush;
}


// ==================================
void BigInt::Run::applicationSimpleSample(std::ostream& o_stream)
{
    std::ostringstream oss;

	SET_START_TEST_NAME(oss);

    // ---------------------------
    BigInt::Rossi rossi1 (100);
    BigInt::Rossi rossi2 ("100", DEC_DIGIT);
    BigInt::Rossi rossi3 ("100", HEX_DIGIT);
    BigInt::Rossi rossi4 ("123456789", DEC_DIGIT);
    BigInt::Rossi rossi5 ("123456789ABCDEF", HEX_DIGIT);
    BigInt::Rossi rossi6 ("99999999999999999999999999999999", DEC_DIGIT);
	BigInt::Rossi rossi7 ("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF", HEX_DIGIT);

    std::vector<BigInt::Rossi> rossiArgs;
    rossiArgs.push_back(rossi1);
    rossiArgs.push_back(rossi2);
    rossiArgs.push_back(rossi3);
    rossiArgs.push_back(rossi4);
    rossiArgs.push_back(rossi5);
    rossiArgs.push_back(rossi6);
    rossiArgs.push_back(rossi7);


    const std::string strRossiDec ("[Rossi: Dec] ");
    const std::string strRossiHex ("[Rossi: Hex] ");

    // ---------------------------
    BigInt::Vin vin1 (100);
    BigInt::Vin vin2 ("100", DEC_DIGIT);
    BigInt::Vin vin3 ("100", HEX_DIGIT);
    BigInt::Vin vin4 ("123456789", DEC_DIGIT);
    BigInt::Vin vin5 ("123456789ABCDEF", HEX_DIGIT);
    BigInt::Vin vin6 ("99999999999999999999999999999999", DEC_DIGIT);
	BigInt::Vin vin7 ("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF", HEX_DIGIT);

    std::vector<BigInt::Vin> vinArgs;
    vinArgs.push_back(vin1);
    vinArgs.push_back(vin2);
    vinArgs.push_back(vin3);
    vinArgs.push_back(vin4);
    vinArgs.push_back(vin5);
    vinArgs.push_back(vin6);
    vinArgs.push_back(vin7);


    const std::string strVinDec ("[Vin: Dec] ");
    const std::string strVinHex ("[Vin: Hex] ");


    // -----------------------------------------------
    for (std::size_t i = 0; i < rossiArgs.size(); i++)
    {
        for (std::size_t k = 0; k < rossiArgs.size(); k++)
        {
            oss << std::endl;

            BigInt::Rossi rossiArg1 = rossiArgs[i];
            BigInt::Rossi rossiArg2 = rossiArgs[k];
            BigInt::Rossi rossiResult;

            std::string theOperation;
            // ---------------------------------------------
            theOperation = "+";
            rossiResult = rossiArg1 + rossiArg2;

            oss << ""
                << strRossiHex
                << rossiArg1 
                << " " 
                << theOperation 
                << " " 
                << rossiArg2 
                << " = " 
                << rossiResult 
                << std::endl;

            oss << ""
                << strRossiDec
                << rossiArg1.toStrDec()  
                << " " 
                << theOperation 
                << " " 
                << rossiArg2.toStrDec()  
                << " = " 
                << rossiResult.toStrDec() 
                << std::endl;

            oss << std::endl;

            // ---------------------------------------------
            if (rossiArg1 >= rossiArg2)
            {
                theOperation = "-";
                rossiResult = rossiArg1 - rossiArg2;

                oss << ""
                    << strRossiHex
                    << rossiArg1 
                    << " " 
                    << theOperation 
                    << " " 
                    << rossiArg2 
                    << " = " 
                    << rossiResult 
                    << std::endl;

                oss << ""
                    << strRossiDec
                    << rossiArg1.toStrDec()  
                    << " " 
                    << theOperation
                    << " " 
                    << rossiArg2.toStrDec()  
                    << " = " 
                    << rossiResult.toStrDec() 
                    << std::endl;

                oss << std::endl;
            }

            // ---------------------------------------------
            theOperation = "*";
            rossiResult = rossiArg1 * rossiArg2;

            oss << ""
                << strRossiHex
                << rossiArg1 
                << " " 
                << theOperation
                << " " 
                << rossiArg2 
                << " = " 
                << rossiResult 
                << std::endl;

            oss << ""
                << strRossiDec
                << rossiArg1.toStrDec()  
                << " " 
                << theOperation 
                << " " 
                << rossiArg2.toStrDec()  
                << " = " 
                << rossiResult.toStrDec() 
                << std::endl;

            oss << std::endl;


            // ---------------------------------------------
            theOperation = "/";
            rossiResult = rossiArg1 / rossiArg2;

            oss << ""
                << strRossiHex
                << rossiArg1 
                << " " 
                << theOperation
                << " " 
                << rossiArg2 
                << " = " 
                << rossiResult 
                << std::endl;

            oss << ""
                << strRossiDec
                << rossiArg1.toStrDec()  
                << " " 
                << theOperation 
                << " " 
                << rossiArg2.toStrDec()  
                << " = " 
                << rossiResult.toStrDec() 
                << std::endl;

            oss << std::endl;


            // ---------------------------------------------
            theOperation = "%";
            rossiResult = rossiArg1 % rossiArg2;

            oss << ""
                << strRossiHex
                << rossiArg1 
                << " " 
                << theOperation
                << " " 
                << rossiArg2 
                << " = " 
                << rossiResult 
                << std::endl;

            oss << ""
                << strRossiDec
                << rossiArg1.toStrDec()  
                << " " 
                << theOperation 
                << " " 
                << rossiArg2.toStrDec()  
                << " = " 
                << rossiResult.toStrDec() 
                << std::endl;

            oss << std::endl;

        } // for (std::size_t k = 0; k < rossiArgs.size(); k++)
    } // for (std::size_t i = 0; i < rossiArgs.size(); i++)

    oss << std::endl;
    // -----------------------------------------------
    for (std::size_t i = 0; i < vinArgs.size(); i++)
    {
        for (std::size_t k = 0; k < vinArgs.size(); k++)
        {
            BigInt::Vin vinArg1 = vinArgs[i];
            BigInt::Vin vinArg2 = vinArgs[k];
            BigInt::Vin vinResult;

            std::string theOperation;
            // ---------------------------------------------
            theOperation = "+";
            vinResult = vinArg1 + vinArg2;

            oss << ""
                << strVinHex
                << vinArg1.toStr0xHex() 
                << " " 
                << theOperation 
                << " " 
                << vinArg2.toStr0xHex() 
                << " = " 
                << vinResult.toStr0xHex() 
                << std::endl;

            oss << ""
                << strVinDec
                << vinArg1.toStrDec()   
                << " " 
                << theOperation 
                << " " 
                << vinArg2.toStrDec() 
                << " = " 
                << vinResult.toStrDec()
                << std::endl;

            oss << std::endl;

        } // for (std::size_t k = 0; k < rossiArgs.size(); k++)
    } // for (std::size_t i = 0; i < rossiArgs.size(); i++)


	SET_FINISH_TEST_NAME(oss);

    o_stream << std::flush << oss.str() << std::flush;
}



// ==================================
void BigInt::Run::runRossiInteractiveSample(std::ostream& o_stream, const std::vector<std::string>& i_args)
{
    std::ostringstream oss;

	SET_START_TEST_NAME(oss);

    ASSERTION (i_args.size() == 4);

    const std::map<std::string, std::string> allowedOperations = getSampleAllowedOperation();
    ASSERTION(!allowedOperations.empty());

    // ---------------------------
    BigInt::Rossi rossiArg1 (std::string(i_args[1]), DEC_DIGIT);
    std::string   op (i_args[2]);
    BigInt::Rossi rossiArg2 (std::string(i_args[3]), DEC_DIGIT);

    BigInt::Rossi rossiResult;

    ASSERTION (allowedOperations.count(op));

    BigInt::Vin vinArg1(rossiArg1);
    BigInt::Vin vinArg2(rossiArg2);

    // ---------------------------
    std::ostringstream ossFailure;
    if (op == "+") 
    {
        rossiResult = rossiArg1 + rossiArg2;
    }

    if (op == "-") 
    {
        if (rossiArg2 <= rossiArg1)
        {
            rossiResult = rossiArg1 - rossiArg2;
        }
        else
        {
            ossFailure << ""
                       << "Arguments are inconsistent for operator"
                       << op
                       << " : "
                       << " minuend = "
                       << vinArg1
                       << ", "
                       << "subtracter = "
                       << vinArg2
                       << ", i.e. minuend < subtracter";        
        }
    }

    if (op == "x") 
    {
        rossiResult = rossiArg1 * rossiArg2;
    }

    if (op == "/") 
    {
        rossiResult = rossiArg1 / rossiArg2;
    }

    if (op == "%") 
    {
        rossiResult = rossiArg1 % rossiArg2;
    }

    // ---------------------------
    BigInt::Vin vinResult(rossiResult);

    if (ossFailure.str().empty())
    {
        oss << ""
            << "[Hex] "
            << rossiArg1 
            << " " 
            << op 
            << " " 
            << rossiArg2 
            << " = " 
            << rossiResult 
            << std::endl;

		
        oss << ""
            << "[Dec] "
            << vinArg1.toStrDec() 
            << " " 
            << op 
            << " " 
            << vinArg2.toStrDec() 
            << " = " 
            << vinResult.toStrDec() 
            << std::endl;

    }
    else
    {
        oss << ""
            << ossFailure.str()
            << std::endl;
    }

	SET_FINISH_TEST_NAME(oss);

    o_stream << std::flush << oss.str() << std::flush;
}


// ==========================================
void BigInt::BaseBigInt::showUnits(std::ostream& o_stream) const
{
    std::ostringstream oss;

    const std::string::size_type widthSpaces = static_cast<std::string::size_type>(BigInt::toString(m_units.size()).size());

    oss << "--- Units: BEGIN ---" 
        << std::endl;
    for (std::size_t i = 0; i < m_units.size(); i++)
    {
        oss << "m_units[" 
            << i 
            << "]"
            << std::string (widthSpaces, ' ')
            << ": " 
            << std::hex
            << std::showbase
            << m_units[i] 
            << std::dec
            << " = "
            << m_units[i] 
            << std::endl;
    }
    oss << "---- Units: END ----" 
        << std::endl;

    
    o_stream << std::flush << oss.str() << std::flush;
}



    


// ================
// class BigInt::BaseBigInt
// ================
// ------------------
bool BigInt::BaseBigInt::isEmpty() const
{
    return m_units.empty();
}



// ------------------
void BigInt::BaseBigInt::maximize()
{
    m_units.clear();
    std::vector<BigInt::Unit>::size_type unitsSize = 0; 

    const BigInt::Unit maxUnit = std::numeric_limits<BigInt::Unit>::max();

    while (true)
    {
        unitsSize = m_units.size();
        try
        {
            m_units.push_back(maxUnit);
        }
        catch(...)
        {
            // Do nothing
            break;
        }
    }
}



// ------------------
std::size_t BigInt::BaseBigInt::getUnitsSize() const
{
    return m_units.size();
}


// ------------------
std::size_t BigInt::BaseBigInt::getHexDigitsInUnits() const
{
    return ((getUnitsSize() * sizeof(Unit) * CHAR_BIT) / NIBBLE_BIT);
}


// ------------------
BigInt::BaseBigInt::~BaseBigInt()
{
}



// =================
// class BigInt::Vin
// =================
// ----------------------------
BigInt::Unit BigInt::Vin::s_carry = 0;
// ----------------------------
BigInt::Unit BigInt::Vin::addUnits(BigInt::Unit n1, BigInt::Unit n2)
{
    n1 += (n2 + s_carry);
    s_carry = (VIN_OVER_BASE2 & n1) || (n1 > VIN_SUB_BASE2);
    return (s_carry ? (n1 - VIN_BASE2) : n1);
}



// ------------------
// Constructor-0
BigInt::Vin::Vin () 
{
}


// ------------------
// Constructor-1
BigInt::Vin::Vin (BigInt::Unit i_unit) 
{
    if (!(i_unit < VIN_BASE2))
    {
        const std::size_t witdh1 = 19;

        std::ostringstream ossErr;
        ossErr  << ""
                << "BigInt::Unit value too big: "
                << std::endl

                << std::setw (witdh1)
                << std::left
                << "i_unit"
                << std::right
                << " = " 
                << std::dec 
                << i_unit 
                << ", " 
                << std::hex 
                << std::showbase
                << i_unit 
                << std::dec
                << std::endl

                << std::setw (witdh1)
                << std::left
                << "VIN_BASE2"
                << std::right
                << " = "
                << VIN_BASE2 
                << ", " 
                << std::hex 
                << std::showbase
                << VIN_BASE2 
                << std::dec 
                << std::endl

                << std::setw (witdh1)
                << std::left
                << "VIN_UNIT_MAX" 
                << std::right
                << " = "
                << VIN_UNIT_MAX 
                << ", " 
                << std::hex 
                << std::showbase
                << VIN_UNIT_MAX 
                << std::dec
                << std::endl
                << std::endl;

        ERR_MSG (std::cerr, ossErr.str());
       
    }
    ASSERTION (i_unit < VIN_BASE2);

    
    // ---------------------
    try
    {
        m_units.reserve(m_units.size() + 1);
    }
    catch(...)
    {
        std::ostringstream ossThrow;
        std::ostringstream ossErr;
        ossErr  << ""
                << "Unable to to do m_units.reserve("
                << (m_units.size() + 1)
                << ")"
                << std::endl;

        // ERR_MSG (std::cerr, ossErr.str());
        ERR_MSG (ossThrow, ossErr.str());
        throw std::runtime_error (ossThrow.str().c_str());
    }

    
    // ---------------------
    try
    {
        m_units.push_back (i_unit);
    }
    catch(...)
    {
        std::ostringstream ossThrow;
        std::ostringstream ossErr;
        ossErr  << ""
                << "Unable to to do m_units.push_back()"
                << "; "
                << "m_units.size() = "
                << m_units.size()
                << std::endl;

        // ERR_MSG (std::cerr, ossErr.str());
        ERR_MSG (ossThrow, ossErr.str());
        throw std::runtime_error (ossThrow.str().c_str());
    }

}



// ------------------
// Constructor-2
BigInt::Vin::Vin (const std::string& i_arg, std::size_t i_digitBase)
{
    bool rc = initViaString (i_arg, i_digitBase);
    ASSERTION (rc);
}

// ------------------
// Constructor-3
BigInt::Vin::Vin (const BigInt::Rossi& i_arg)
{
    const std::string str (i_arg.toStrHex ());
    bool rc = initViaString (str, HEX_DIGIT);
    ASSERTION (rc);
}



// ------------------
long double BigInt::Vin::toDouble () const
{
	if (m_units.empty()) 
    {
		return static_cast<long double>(0);
    }

    ASSERTION (!m_units.empty());

    // --------------
	double ret = 0.0;
	const long double factor = static_cast<long double>(std::numeric_limits<Unit>::max()) + 1.0; 
    for (std::size_t i1 = (m_units.size() - 1); i1; i1--)
    {
		const std::size_t i = i1 - 1;
		ret = ret * factor;
        ret = ret + static_cast<long double>(m_units [i]);
    }
    return ret;
    return ret;
}


// -----------------------
BigInt::Vin BigInt::Vin::fromDouble (const long double& i_double)
{
	BigInt::Vin ret;
    long double majorPart = i_double;
	long double minorPart = 0.0;

	const long double epsilon = 0.1;
	const long double denom = static_cast<long double>(std::numeric_limits<Unit>::max()) + 1.0;
	
	while (true)
	{
		minorPart = std::fmod (majorPart, denom);
		ret.m_units.push_back(Unit(minorPart));

		majorPart = majorPart / denom;

		if (majorPart < epsilon)
		{
			break;
		}
	}

	return ret;

}

// ------------------
BigInt::Unit BigInt::Vin::toUnit () const
{
    ASSERTION (!m_units.empty());
    if (m_units.size() > 1)
    {
        std::ostringstream ossErr;
        ossErr  << ""
                << "BigInt::Unit can't be obtained: m_units.size() too big"
                << std::endl
                << "m_units.size() = " 
                << m_units.size() 
                << std::endl
                << "Here m_units.size() must be = 1"
                << std::endl
                << std::endl;

        ERR_MSG (std::cerr, ossErr.str());
        ASSERTION (m_units.size() == 1);
    }
    ASSERTION (m_units.size() == 1);
    return m_units.front();
}

// ------------------
// BigInt::Vin::operator BigInt::Unit () const
// {
//  return toUnit();
// }


// ------------------
bool BigInt::Vin::initViaString (const std::string& i_arg, std::size_t i_digitBase)
{
    ASSERTION (m_units.empty());
    ASSERTION ((i_digitBase == BigInt::HEX_DIGIT) || (i_digitBase == BigInt::DEC_DIGIT));

    m_units.push_back(0);

    for (std::size_t i = 0; i < i_arg.size(); i++)
    {
        switch (i_digitBase)
        {
            case DEC_DIGIT:
                if (!isdigit (i_arg[i])) 
                {
                    std::ostringstream ossErr;
                    ossErr  << ""
                            << "std::string contains non-decimal digit"
                            << std::endl
                            << "std::string = " 
                            << i_arg 
                            << std::endl
                            << i 
                            << "-th char = " 
                            << i_arg[i] 
                            << std::endl
                            << std::endl;

                    ERR_MSG (std::cerr, ossErr.str());
              
                    ASSERTION (0);
                    return false; // unused
                }
                break;

            case HEX_DIGIT:
                if (!isxdigit (i_arg[i])) 
                {
                    std::ostringstream ossErr;
                    ossErr  << ""
                            << "std::string contains non-hexadecimal digit"
                            << std::endl
                            << "std::string = " 
                            << i_arg 
                            << std::endl
                            << i 
                            << "-th char = " 
                            << i_arg[i] 
                            << std::endl
                            << std::endl;

                    ERR_MSG (std::cerr, ossErr.str());
                    ASSERTION (0);
                    return false; // unused
                }
                break;

            default:
                ASSERTION (0);
                return false;
                break; // unused
        }  // switch (i_digitBase)
    } // for (std::size_t i = 0; i < i_arg.size(); i++)

    for (std::size_t i = 0; i < i_arg.size(); i++)
    {
        *this = (*this) * static_cast<BigInt::Unit>(i_digitBase)
                + 
                BigInt::Vin (static_cast<BigInt::Unit>(ms_hex2dec[i_arg[i]]));
    }
    return true;
}



// ------------------
BigInt::Vin BigInt::Vin::operator+ (const BigInt::Vin& i_arg) const
{
    BigInt::Vin ret;
    const std::size_t maxSize (std::max (m_units.size (), i_arg.m_units.size ()));

    std::vector<BigInt::Unit> u1 (m_units);
    std::vector<BigInt::Unit> u2 (i_arg.m_units);

    
    // ---------------------
    try
    {
        u1.reserve(maxSize);
    }
    catch(...)
    {
        std::ostringstream ossThrow;
        std::ostringstream ossErr;
        ossErr  << ""
                << "Unable to to do u1.reserve("
                << maxSize
                << ")"
                << std::endl;

        // ERR_MSG (std::cerr, ossErr.str());
        ERR_MSG (ossThrow, ossErr.str());
        throw std::runtime_error (ossThrow.str().c_str());
    }

    // ---------------------
    try
    {
        u2.reserve(maxSize);
    }
    catch(...)
    {
        std::ostringstream ossThrow;
        std::ostringstream ossErr;
        ossErr  << ""
                << "Unable to to do u2.reserve("
                << maxSize
                << ")"
                << std::endl;

        // ERR_MSG (std::cerr, ossErr.str());
        ERR_MSG (ossThrow, ossErr.str());
        throw std::runtime_error (ossThrow.str().c_str());
    }

    
    // ---------------------
    try
    {
        ret.m_units.reserve(maxSize + 1);
    }
    catch(...)
    {
        std::ostringstream ossThrow;
        std::ostringstream ossErr;
        ossErr  << ""
                << "Unable to to do ret.m_units.reserve("
                << (maxSize + 1)
                << ")"
                << std::endl;

        // ERR_MSG (std::cerr, ossErr.str());
        ERR_MSG (ossThrow, ossErr.str());
        throw std::runtime_error (ossThrow.str().c_str());
    }

    // ---------------------
    try
    {
        u1.resize(maxSize);
    }
    catch(...)
    {
        std::ostringstream ossThrow;
        std::ostringstream ossErr;
        ossErr  << ""
                << "Unable to to do u1.resize("
                << maxSize
                << ")"
                << std::endl;

        // ERR_MSG (std::cerr, ossErr.str());
        ERR_MSG (ossThrow, ossErr.str());
        throw std::runtime_error (ossThrow.str().c_str());
    }

    // ---------------------
    try
    {
        u2.resize(maxSize);
    }
    catch(...)
    {
        std::ostringstream ossThrow;
        std::ostringstream ossErr;
        ossErr  << ""
                << "Unable to to do u2.resize("
                << maxSize
                << ")"
                << std::endl;

        // ERR_MSG (std::cerr, ossErr.str());
        ERR_MSG (ossThrow, ossErr.str());
        throw std::runtime_error (ossThrow.str().c_str());
    }

    // ---------------------
    try
    {
        ret.m_units.resize(maxSize);
    }
    catch(...)
    {
        std::ostringstream ossThrow;
        std::ostringstream ossErr;
        ossErr  << ""
                << "Unable to to do ret.m_units.resize("
                << maxSize
                << ")"
                << std::endl;

        // ERR_MSG (std::cerr, ossErr.str());
        ERR_MSG (ossThrow, ossErr.str());
        throw std::runtime_error (ossThrow.str().c_str());
    }



    s_carry = 0;
    std::transform (&u1[0], &u1[0] + maxSize, &u2[0], &ret.m_units[0], BigInt::Vin::addUnits);

  
    if (s_carry) 
    {
        try
        {
            ret.m_units.push_back (s_carry);
        }
        catch(...)
        {
            std::ostringstream ossThrow;
            std::ostringstream ossErr;
            ossErr  << ""
                    << "Unable to to do ret.m_units.push_back()"
                    << "; "
                    << "ret.m_units.size() = "
                    << ret.m_units.size()
                    << std::endl;

            // ERR_MSG (std::cerr, ossErr.str());
            ERR_MSG (ossThrow, ossErr.str());
            throw std::runtime_error (ossThrow.str().c_str());
        }

    }

    return ret;

}


// ------------------
BigInt::Vin BigInt::Vin::operator* (BigInt::Unit i_arg) const
{
    BigInt::Vin ret (0);
    for (std::size_t i = 0; i < i_arg; i++) 
    {
        ret = ret + (*this);
    }
    return ret;
}


// ------------------
bool BigInt::Vin::operator< (const BigInt::Vin& i_arg) const
{
    if (m_units.size() < i_arg.m_units.size())
    {
        return true;
    }
    if (i_arg.m_units.size() < m_units.size())
    {
        return false;
    }

    ASSERTION (m_units.size() == i_arg.m_units.size());
    for (std::size_t i = (m_units.size() - 1); i > 0; i--)
    {
        if (m_units[i] < i_arg.m_units[i])
        {
            return true;
        }

        if (i_arg.m_units[i] < m_units[i])
        {
            return false;
        }
    }
    return (m_units[0] < i_arg.m_units[0]);
}


// ------------------
bool BigInt::Vin::operator<= (const BigInt::Vin& i_arg) const
{
    if (*this < i_arg) 
    {
        return true;
    }

    if (*this == i_arg) 
    {
        return true;
    }

    return false;
}



// ------------------
bool BigInt::Vin::operator> (const BigInt::Vin& i_arg) const
{
    return (!(*this <= i_arg));
}


// ------------------
bool BigInt::Vin::operator>= (const BigInt::Vin& i_arg) const
{
    return (!(*this < i_arg));
}

// ------------------
bool BigInt::Vin::operator== (const BigInt::Vin& i_arg) const
{
    if (*this < i_arg)
    {
        return false;
    }

    if (i_arg < *this) 
    {
        return false;
    }

    return true;
}

// ------------------
bool BigInt::Vin::operator!= (const BigInt::Vin& i_arg) const
{
    return (!(*this == i_arg));
}


// ------------------
std::string BigInt::Vin::toStrHex (const std::string& i_prefix) const
{
    std::ostringstream oss;

    BigInt::Rossi rossi (toStrDec(), BigInt::DEC_DIGIT);

    oss << i_prefix
        << rossi.toStrHex();

    return oss.str();
}


// ------------------
std::string BigInt::Vin::toStr0xHex () const
{
    return toStrHex("0x");
}


// ------------------
std::string BigInt::Vin::toStrDec (const std::string& i_prefix) const
{
    std::ostringstream oss;

    if (isEmpty ()) 
    {
        return "isEmpty";
    }

    oss << i_prefix;

    for (std::size_t i = (m_units.size() - 1); i; --i) 
    {
        oss << m_units [i] 
            << std::setw (BigInt::VIN_BASE1 - 1) 
            << std::setfill ('0');
    }
    oss << m_units [0];

    return oss.str();
}



// ------------------
std::size_t BigInt::Vin::getActualHexDigits () const
{
    const std::string str (toStrHex());

	return str.size();
}

// ------------------
std::size_t BigInt::Vin::getActualDecDigits () const
{
	const std::string str (toStrDec());

	return str.size(); 
}


// ------------------
std::ostream& operator<< (std::ostream& o_os, const BigInt::Vin& i_ins)
{
	
    return o_os << i_ins.toStr0xHex();
	// return o_os << i_ins.toStrDec();
}


// ==========================================
void BigInt::Vin::showUnits(std::ostream& o_stream) const
{
    std::ostringstream oss;

    BigInt::Rossi rossi(*this);

    oss << std::endl;
    oss << ""
        << "BigInt::Vin value"
        << ": "
        << rossi.toStr0xHex()
        << " = "       
        << this->toStrDec()
        << std::endl;

    BigInt::BaseBigInt::showUnits(oss);

    oss << std::endl;

    o_stream << std::flush << oss.str() << std::flush;
}


// ===================
// class BigInt::Rossi
// ===================


// ------------------
// Constructor-0
BigInt::Rossi::Rossi () 
{
    ASSERTION(isEmpty());
}


// ------------------
// Constructor-1
BigInt::Rossi::Rossi (BigInt::Unit i_unit) 
{
    ASSERTION (m_units.empty());
    m_units.push_back (i_unit);
}


// ------------------
// Constructor-2
BigInt::Rossi::Rossi (const std::string& i_arg, std::size_t i_digitBase) 
{
    const bool rc = initViaString (i_arg, i_digitBase);
    ASSERTION (rc);
}



// ------------------
// Constructor-3
BigInt::Rossi::Rossi (const BigInt::Vin& i_arg)
{
    std::ostringstream oss;
    oss << i_arg;
    const bool rc = initViaString (oss.str(), DEC_DIGIT);
    ASSERTION (rc);
}


// ------------------
// Constructor-4
BigInt::Rossi::Rossi (const std::size_t i_noOfUnits, BigInt::Unit i_internalUnits, BigInt::Unit i_backUnit, const std::string& i_msg)
{
    ASSERTION (m_units.empty());

    try
    {
        m_units.resize (i_noOfUnits, i_internalUnits);      
    }
    catch(...)
    {
        std::ostringstream ossThrow;
        std::ostringstream ossErr;
        ossErr  << ""
                << "Unable to do m_units.resize("
                << i_noOfUnits
                << ")"
				<< i_msg
                << std::endl;

            // ERR_MSG (std::cerr, ossErr.str());
            ERR_MSG (ossThrow, ossErr.str());
            throw std::runtime_error (ossThrow.str().c_str());
    }

    m_units.back() = i_backUnit;
}


// ------------------
long double BigInt::Rossi::toDouble () const
{
    if (m_units.empty()) 
    {
		return static_cast<long double>(0);
    }

    ASSERTION (!m_units.empty());

	const long double factor = static_cast<long double>(std::numeric_limits<Unit>::max()) + 1.0; 

    // --------------
	long double ret = 0.0;
    for (std::size_t i1 = m_units.size(); i1; i1--)
    {
		ret = ret * factor;
        ret = ret + static_cast<long double>(m_units [i1 - 1]);
    }

    return ret;
}


// --------------------------------
BigInt::Rossi BigInt::Rossi::fromDouble (const long double& i_double)
{
	BigInt::Rossi ret;
    long double majorPart = i_double;
	long double minorPart = 0.0;

	const long double epsilon = 0.1; 
	const long double denom = static_cast<long double>(std::numeric_limits<Unit>::max()) + 1.0;

	if (majorPart < epsilon)
	{
		return BigInt::Rossi(0);
	}
    
	while (true)
	{
		minorPart = std::fmod (majorPart, denom);

		ret.m_units.push_back(Unit(minorPart));

		majorPart = majorPart / denom;
		
		if (majorPart < epsilon)
		{
			break;
		}
	}
	
	ret.smartTruncateUnits();
	
	return ret;
}


// ------------------
BigInt::Unit BigInt::Rossi::toUnit () const
{
    ASSERTION (!m_units.empty());
    if (m_units.size() > 1)
    {
        std::ostringstream ossErr;
        ossErr  << ""
                << "BigInt::Unit can't be obtained: m_units.size() too big"
                << std::endl
                << "m_units.size() = " 
                << m_units.size() 
                << std::endl
                << "Hex value = " 
                << toStrHex() 
                << std::endl
                << "Here m_units.size() must be = 1"
                << std::endl
                << std::endl;

        ERR_MSG (std::cerr, ossErr.str());
        ASSERTION (m_units.size() == 1);
    }
    ASSERTION (m_units.size() == 1);
    return m_units.front();
}

// ------------------
// BigInt::Rossi::operator BigInt::Unit () const
// {
//  return toUnit();
// }


// ------------------
bool BigInt::Rossi::initViaString (const std::string& i_arg, std::size_t i_digitBase)
{
    ASSERTION (m_units.empty());
    ASSERTION ((i_digitBase == BigInt::HEX_DIGIT) || (i_digitBase == BigInt::DEC_DIGIT));

    m_units.push_back(0);

    for (std::size_t i = 0; i < i_arg.size(); i++)
    {
        switch (i_digitBase)
        {
            case DEC_DIGIT:
                if (!isdigit (i_arg[i])) 
                {
                    std::ostringstream ossErr;
                    ossErr  << ""
                            << "std::string contains non-decimal digit"
                            << std::endl
                            << "std::string = " 
                            << i_arg 
                            << std::endl
                            << i 
                            << "-th char = " 
                            << i_arg[i] 
                            << std::endl
                            << std::endl;

                    ERR_MSG (std::cerr, ossErr.str());

                    ASSERTION (0);
                    return false; // unused
                }
                break;

            case HEX_DIGIT:
                if (!isxdigit (i_arg[i])) 
                { 
                    std::ostringstream ossErr;
                    ossErr  << ""
                            << "std::string contains non-hexadecimal digit"
                            << std::endl
                            << "std::string = " 
                            << i_arg 
                            << std::endl
                            << i 
                            << "-th char = " 
                            << i_arg[i] 
                            << std::endl
                            << std::endl;

                    ERR_MSG (std::cerr, ossErr.str());
                    ASSERTION (0);
                    return false; // unused
                }
            break;

        default:
            ASSERTION (0);
            return false;
            break; // unused

        }
    }

    for (std::size_t i = 0; i < i_arg.size(); i++)
    {
        *this = (*this) * static_cast<BigInt::Unit>(i_digitBase)
                + 
                BigInt::Rossi (static_cast<BigInt::Unit>(ms_hex2dec[i_arg[i]]));
    }
    return true;
}


// ------------------
void BigInt::Rossi::resizeUnits(std::size_t i_size)
{
    m_units.resize(i_size);
}

// ------------------
void BigInt::Rossi::truncateUnits()
{
    while ((m_units.size() > 1) && (m_units.back() == 0))
    {
        m_units.pop_back();
    } 
    ASSERTION(!isEmpty());

}


// ------------------
void BigInt::Rossi::smartTruncateUnits()
{
    if (backUnitIsNull ())
    { 
  	    truncateUnits ();
    }
}

// ------------------
bool BigInt::Rossi::backUnitIsNull () const
{
    ASSERTION (!m_units.empty());
    if (m_units.size() == 1) 
    {
        return false;
    }
    return (m_units.back() == 0);
}



// ------------------
// BigInt::Rossi& BigInt::Rossi::operator= (BigInt::Unit i_arg)
// {
//  *this = BigInt::Rossi (i_arg);
//  return *this;
// }

// ------------------
BigInt::Rossi BigInt::Rossi::operator+ (const BigInt::Rossi& i_arg)
{
    BigInt::Rossi ret (0);
    BigInt::Rossi arg (i_arg);
    BigInt::Unit carry = 0;

    const std::size_t maxSize (std::max (getUnitsSize(), arg.getUnitsSize()));

    resizeUnits (maxSize + 1);
    arg.resizeUnits (maxSize + 1);
    ret.resizeUnits (maxSize + 1);

    for (std::size_t i = 0; i < m_units.size(); i++)
    {  	 
        ret.m_units[i] = m_units[i] + arg.m_units[i] + carry;
        if (carry == 0)
        {
            carry = ((ret.m_units[i] < m_units[i] || ret.m_units[i] < arg.m_units[i]) ? 1 : 0);
        }
        else
        {
            carry = ((ret.m_units[i] <= m_units[i] || ret.m_units[i] <= arg.m_units[i]) ? 1 : 0);
        }
    }

    smartTruncateUnits();
    ret.smartTruncateUnits();

    return ret;

}

// ------------------
BigInt::Rossi BigInt::Rossi::operator+ (BigInt::Unit i_arg)
{
    return (*this + BigInt::Rossi (i_arg));
}


// ------------------
bool BigInt::Rossi::operator< (const BigInt::Rossi& i_arg) const
{
    if (m_units.size() < i_arg.m_units.size()) 
    {
        return true;
    }
    if (i_arg.m_units.size() < m_units.size())
    {
        return false;
    }

    ASSERTION (m_units.size() == i_arg.m_units.size());
    for (std::size_t i = (m_units.size() - 1); i > 0; i--)
    {
        if (m_units[i] < i_arg.m_units[i]) 
        {
            return true;
        }

        if (i_arg.m_units[i] < m_units[i]) 
        {
            return false;
        }
    }
    return (m_units[0] < i_arg.m_units[0]);
}


// ------------------
bool BigInt::Rossi::operator<= (const BigInt::Rossi& i_arg) const
{

    if (*this < i_arg) 
    {
        return true;
    }
    if (*this == i_arg) 
    {
        return true;
    }

    return false;
}



// ------------------
bool BigInt::Rossi::operator> (const BigInt::Rossi& i_arg) const
{
  return (!(*this <= i_arg));
}


// ------------------
bool BigInt::Rossi::operator>= (const BigInt::Rossi& i_arg) const
{
    return (!(*this < i_arg));
}

// ------------------
bool BigInt::Rossi::operator== (const BigInt::Rossi& i_arg) const
{
    if (*this < i_arg) 
    {
        return false;
    }

    if (i_arg < *this) 
    {
        return false;
    }

    return true;
}


// ------------------
bool BigInt::Rossi::operator!= (const BigInt::Rossi& i_arg) const
{
    return (!(*this == i_arg));
}



// ------------------
BigInt::Rossi BigInt::Rossi::operator/ (const BigInt::Rossi& i_arg) const
{
    return divide(*this, i_arg, NULL);
}


// ------------------
BigInt::Rossi BigInt::Rossi::operator% (const BigInt::Rossi& i_arg) const
{
    BigInt::Rossi ret;
    divide(*this, i_arg, &ret);
    return ret;
}



// ------------------
BigInt::Rossi BigInt::Rossi::operator>> (std::size_t i_shift)
{
    BigInt::Rossi tmp;
    BigInt::Rossi ret;

    tmp = *this;

    ret.resizeUnits (m_units.size());

    ASSERTION (ret.getUnitsSize() == tmp.getUnitsSize());

    for (std::size_t i = 0; i < i_shift; i++)
    {
        ret.m_units.back() = (tmp.m_units.back() >> 1);

        for (std::size_t j1 = tmp.m_units.size(); j1 ; j1--)
        {
            const std::size_t j = j1 -1;
            ret.m_units[j] = (tmp.m_units[j] >> 1);

            if ((tmp.m_units[j + 1] & 1) != 0)
            {
                ret.m_units[j] |= UNIT_MSB;      // Set MSB bit
            }
        }
        tmp = ret;
    }

    smartTruncateUnits();
    ret.smartTruncateUnits();

    return ret;
}


// ------------------
BigInt::Rossi BigInt::Rossi::operator<< (std::size_t i_shift)
{
    BigInt::Rossi tmp;
    BigInt::Rossi ret;

    tmp = *this;

    ret.resizeUnits (m_units.size() + 1);

    ASSERTION ((ret.getUnitsSize() + 1) == tmp.getUnitsSize());

    for (std::size_t i = 0; i < i_shift; i++)
    {
        ret.m_units.front() = (tmp.m_units.front() << 1);
        for (std::size_t j = 1; j < tmp.m_units.size(); j++)
        {
            ret.m_units[j] = (tmp.m_units[j] << 1);
            if ((tmp.m_units[j-1] & UNIT_MSB) != 0)
            {
                ret.m_units[j] |= 1;     // Set MSB bit
            }
        }
        if ((tmp.m_units.back() & UNIT_MSB) != 0)
        {
            ret.m_units.back() |= 1;   // Set MSB bit
        }
        tmp = ret;
    }

    smartTruncateUnits();
    ret.smartTruncateUnits();

    return ret;
}



// ------------------
BigInt::Rossi& BigInt::Rossi::operator>>= (std::size_t i_shift)
{
    BigInt::Unit carry;
    m_units.push_back(0);

    for (std::size_t i = 0; i < i_shift; i++)
    {
        carry = m_units.back() & 1;
        m_units.back() >>= 1;

        for (std::size_t j1 = m_units.size(); j1; j1--)
        {
            const std::size_t j = j1 -1;
            if (carry)
            {
                carry = m_units[j] & 1;
                m_units[j] >>= 1;
                m_units[j] |= UNIT_MSB;
            }
            else
            {
                carry = m_units[j] & 1;
                m_units[j] >>= 1;
            }
        }
    }

    smartTruncateUnits();

    return *this;
}



// ------------------
BigInt::Rossi& BigInt::Rossi::operator<<= (std::size_t i_shift)
{
    BigInt::Unit carry;

    const std::size_t pushBackSize (i_shift/(sizeof (std::size_t) * CHAR_BIT) + 1);

    for (std::size_t i = 0; i < (pushBackSize + 1); i++)
    {
        try
        {
            m_units.push_back(0);        
        }
        catch(...)
        {
            std::ostringstream ossThrow;
            std::ostringstream ossErr;
            ossErr  << ""
                    << "Unable to to do m_units.push_back()"
                    << "; "
                    << "m_units.size() = "
                    << m_units.size()
                    << std::endl;

            // ERR_MSG (std::cerr, ossErr.str());
            ERR_MSG (ossThrow, ossErr.str());
            throw std::runtime_error (ossThrow.str().c_str());
        }
    }

    for (std::size_t i = 0; i < i_shift; i++)
    {
        carry = m_units.front() & UNIT_MSB;
        m_units.front() <<= 1;

        for (std::size_t j = 1; j < m_units.size(); j++)
        {
            if (carry)
            {
                carry = m_units[j] & UNIT_MSB;
                m_units[j] <<= 1;
                m_units[j] |= 1;
            }
            else
            {
                carry = m_units[j] & UNIT_MSB;
                m_units[j] <<= 1;
            }
        }
    }

    smartTruncateUnits();

    return *this;
}



// ------------------
BigInt::Rossi& BigInt::Rossi::operator+=(const BigInt::Rossi& i_arg)
{
    BigInt::Unit carry = 0;
    BigInt::Unit prevDigit;
    BigInt::Rossi arg (i_arg);

    const std::size_t maxSize (std::max (getUnitsSize(), arg.getUnitsSize()));

    resizeUnits (maxSize + 1);
    arg.resizeUnits (maxSize + 1);

    for (std::size_t i = 0; i < m_units.size(); i++)
    {
        prevDigit = m_units[i];
        m_units[i] = m_units[i] + arg.m_units[i] + carry;
        if (carry == 0)
        {
            carry = ((m_units[i] < prevDigit || m_units[i] < arg.m_units[i]) ? 1 : 0);
        }
        else
        {
        carry = ((m_units[i] <= prevDigit || m_units[i] <= arg.m_units[i]) ? 1 : 0);
        }
    }

    smartTruncateUnits();
    return *this;
}


// ------------------
BigInt::Rossi& BigInt::Rossi::operator++()  // Pre Increment operator -- faster than add
{
    m_units.push_back(0);

    m_units.front()++;
    for (std::size_t i = 1; i < m_units.size(); i++)
    {
        if (m_units[i-1]) 
        {
            break;
        }
        m_units[i]++;
    }

    smartTruncateUnits();

    return *this;
}


// ------------------
BigInt::Rossi BigInt::Rossi::operator++ (int)  // Post Increment operator -- faster than add
{
    BigInt::Rossi tmp (*this);
    ++*this;
    return tmp;
}




// ------------------
BigInt::Rossi BigInt::Rossi::operator- ()  // Negates a number
{
    BigInt::Rossi ret;
    ret.resizeUnits(m_units.size() + 1);

    for (std::size_t i = 0; i < m_units.size(); i++)
    {
        ret.m_units[i] = ~m_units[i];
    }

    ret = ret + RossiOne;

    smartTruncateUnits();
    ret.smartTruncateUnits();

    return ret;
}



// ------------------
BigInt::Rossi BigInt::Rossi::operator-(const BigInt::Rossi& i_arg)
{
    BigInt::Rossi ret (RossiZero);
    BigInt::Rossi arg (i_arg);
    BigInt::Unit borrow = 0;

    const std::size_t maxSize (std::max (getUnitsSize(), arg.getUnitsSize()));

    resizeUnits (maxSize + 1);
    arg.resizeUnits (maxSize + 1);
    ret.resizeUnits (maxSize + 1);

    if (*this < arg)
    {
        std::ostringstream ossErr;
        ossErr  << ""
                << "minuend < subtracter"
                << std::endl
                << "minuend    = " 
                << this->toStrHex() 
				<< ", "
                << "subtracter = " 
                << arg.toStrHex() 
                << std::endl
                << std::endl;

        ERR_MSG (std::cerr, ossErr.str());
        ASSERTION (0);
    }

    for (std::size_t i = 0; i < m_units.size(); i++)
    {
        ret.m_units[i] = m_units[i] - arg.m_units[i] - borrow;
    
        if (borrow == 0)
        {
            borrow = ((m_units[i] < arg.m_units[i]) ? 1 : 0);
        }
        else
        {
        borrow = ((m_units[i] <= arg.m_units[i]) ? 1 : 0);
        }
    }

    smartTruncateUnits();
    ret.smartTruncateUnits();

    return ret;
}


// ------------------
BigInt::Rossi& BigInt::Rossi::operator-= (const BigInt::Rossi& i_arg)
{
    BigInt::Unit borrow = 0;
    BigInt::Rossi arg (i_arg);
    BigInt::Unit prevDigit;

    const std::size_t maxSize (std::max (getUnitsSize(), arg.getUnitsSize()));
	resizeUnits (maxSize + 1);
    arg.resizeUnits (maxSize + 1);
  
    if (*this < arg)
    {
        std::ostringstream ossErr;
        ossErr  << ""
                << "minuend < subtracter"
                << std::endl
                << "minuend    = " 
                << this->toStrHex() 
                << ", "
				<< "subtracter = " 
                << arg.toStrHex() 
                << std::endl
                << std::endl;

        ERR_MSG (std::cerr, ossErr.str());
        ASSERTION (0);
    }

    for (std::size_t i = 0; i < m_units.size(); i++)
    {
        prevDigit = m_units[i];
        m_units[i] = m_units[i] - arg.m_units[i] - borrow;

        if (borrow == 0)
        {
            borrow = ((prevDigit < arg.m_units[i]) ? 1 : 0);
        }
        else
        {
            borrow = ((prevDigit <= arg.m_units[i]) ? 1 : 0);
        }
    }

    smartTruncateUnits();

    return *this;
}




// ------------------
BigInt::Rossi& BigInt::Rossi::operator--()  // Pre Decrement operator -- faster than add
{
    m_units.front()--;
    for (std::size_t i = 1; i < m_units.size(); i++)
    {
        if (m_units[i-1] != UNIT_MAX) 
        {
            break;
        }
        ASSERTION (m_units[i-1] == UNIT_MAX);

        m_units[i]--;
    }

    smartTruncateUnits();
    return *this;
}


// ------------------
BigInt::Rossi BigInt::Rossi::operator-- (int)  // Post Decrement operator -- faster than add
{
    BigInt::Rossi tmp (*this);
    --*this;
    return tmp;
}



// ------------------
BigInt::Rossi BigInt::Rossi::operator& (const BigInt::Rossi& i_arg)
{
    const std::size_t maxSize (std::max (getUnitsSize (), i_arg.getUnitsSize ()));

    BigInt::Rossi ret;
    BigInt::Rossi arg (i_arg);

    ret.resizeUnits(maxSize);
    arg.resizeUnits(maxSize);
    resizeUnits(maxSize);

    for (std::size_t i1 = m_units.size(); i1; i1--)
    {
        const std::size_t i = i1 - 1;
        ret.m_units[i] = m_units[i] & arg.m_units[i];
    }

    smartTruncateUnits();
    ret.smartTruncateUnits();

    return ret;
}


// ------------------
BigInt::Rossi BigInt::Rossi::operator| (const BigInt::Rossi& i_arg)
{
    const std::size_t maxSize (std::max (getUnitsSize (), i_arg.getUnitsSize ()));

    BigInt::Rossi ret;
    BigInt::Rossi arg (i_arg);

    ret.resizeUnits(maxSize);
    arg.resizeUnits(maxSize);
    resizeUnits(maxSize);

    for (std::size_t i1 = m_units.size(); i1; i1--)
    {
        const std::size_t i = i1 - 1;
        ret.m_units[i] = m_units[i] | arg.m_units[i];
    }

   
    smartTruncateUnits();
    ret.smartTruncateUnits();

    return ret;
}



// ------------------
BigInt::Rossi BigInt::Rossi::operator^ (const BigInt::Rossi& i_arg)
{
    const std::size_t maxSize (std::max (getUnitsSize (), i_arg.getUnitsSize ()));

    BigInt::Rossi ret;
    BigInt::Rossi arg (i_arg);

    ret.resizeUnits(maxSize);
    arg.resizeUnits(maxSize);
    resizeUnits(maxSize);

    for (std::size_t i1 = m_units.size(); i1; i1--)     
    {
        const std::size_t i = i1 - 1;
        ret.m_units[i] = m_units[i] ^ arg.m_units[i];
    }

   
    smartTruncateUnits();
    ret.smartTruncateUnits();

    return ret;

}


// ------------------
BigInt::Rossi BigInt::Rossi::operator~ ()
{
    BigInt::Rossi ret;

    ret.resizeUnits(getUnitsSize());

    for (std::size_t i1 = m_units.size(); i1; i1--)
    {
        const std::size_t i = i1 - 1;
        ret.m_units[i] = ~m_units[i];
    }

   
    smartTruncateUnits();
    ret.smartTruncateUnits();

    return ret;

}



// ------------------
BigInt::Rossi& BigInt::Rossi::operator&= (const BigInt::Rossi& i_arg)
{
    const std::size_t maxSize (std::max  (getUnitsSize (), i_arg.getUnitsSize ()));

    BigInt::Rossi arg (i_arg);

    arg.resizeUnits(maxSize);
    resizeUnits(maxSize);

    for (std::size_t i1 = m_units.size(); i1; i1--) 
    {
        const std::size_t i = i1 - 1; 
        m_units[i] = m_units[i] & arg.m_units[i];
    }

    smartTruncateUnits();
 
    return *this;

}


// ------------------
BigInt::Rossi& BigInt::Rossi::operator|=(const BigInt::Rossi& i_arg)
{
    const std::size_t maxSize (std::max (getUnitsSize (), i_arg.getUnitsSize ()));

    BigInt::Rossi arg (i_arg);

    arg.resizeUnits(maxSize);
    resizeUnits(maxSize);

    for (std::size_t i1 = m_units.size(); i1; i1--)
    {
        const std::size_t i = i1 - 1; 
        m_units[i] = m_units[i] | arg.m_units[i];
    }

    smartTruncateUnits();

    return *this;

}


// ------------------
BigInt::Rossi& BigInt::Rossi::operator^=(const BigInt::Rossi& i_arg)
{
    const std::size_t maxSize (std::max  (getUnitsSize (), i_arg.getUnitsSize ()));

    BigInt::Rossi arg (i_arg);

    arg.resizeUnits(maxSize);
    resizeUnits(maxSize);

    for (std::size_t i1 = m_units.size(); i1; i1--)
    {
        const std::size_t i = i1 - 1; 
        m_units[i] = m_units[i] ^ arg.m_units[i];
    }

    smartTruncateUnits();
    return *this;

}




// ------------------
BigInt::Rossi BigInt::Rossi::operator* (BigInt::Rossi i_arg) const
{
    BigInt::Rossi tmp;
    BigInt::Rossi ret;

 
    tmp = *this;
    ret = RossiZero;
    // ret.resizeUnits (getUnitsSize () + i_arg.getUnitsSize ());
  
    do
    {	
        if ((i_arg.m_units.front() & 1) != 0) 
        {
            ret += tmp;
        }
        i_arg >>= 1;
        tmp <<= 1;
    } while (i_arg != RossiZero);

    const_cast<BigInt::Rossi*>(this)->smartTruncateUnits();
    ret.smartTruncateUnits();

    return ret;
}


// ------------------
BigInt::Rossi BigInt::Rossi::operator* (BigInt::Unit i_arg) const
{
    return ((*this) * BigInt::Rossi (i_arg));
}

				 

// ------------------
BigInt::Rossi BigInt::Rossi::divide (
		                    const BigInt::Rossi& i_dividend, 
		                    const BigInt::Rossi& i_divisor, 
		                    BigInt::Rossi*       o_remainder
		                    ) const
{
    BigInt::Rossi dividend (i_dividend); 
    BigInt::Rossi divisor (i_divisor); 


    long shiftcnt (0);

    // --- Check for attempt to divide by zero ---
    if (divisor == RossiZero)
    {
        ERR_MSG (std::cerr, "divisor == Zero");
        ASSERTION (0 && "divisor == Zero");

        shiftcnt = 1 / shiftcnt;  // Force a divide by zero exception. (shiftcnt=0)
    }

    BigInt::Rossi quotient (RossiZero);

    quotient.resizeUnits (dividend.getUnitsSize ());

    if (o_remainder != NULL)
    {
        o_remainder->resizeUnits (dividend.getUnitsSize ());
    }


    // --- Left shift divisor until it is greater than or equal to dividend ---
    // while ( divisor < dividend && ((divisor.m_units.back() & UNIT_MSB) == 0))
    while ( divisor < dividend)
    { 	
        divisor <<= 1;
		ASSERTION (shiftcnt >= 0);
        shiftcnt++;
		ASSERTION (shiftcnt >= 0);
    }

    if (divisor > dividend)      // If divisor is greater than dividend, right shift divisor
    {
        divisor >>= 1;
		ASSERTION (shiftcnt >= 0);
        shiftcnt--; 
		// ASSERTION (shiftcnt >= 0);
    }

    if (shiftcnt >= 0)
    { 	
        for(long i = 0; i <= shiftcnt; i++)
        {    
			ASSERTION (i >= 0);
			ASSERTION (i < (std::numeric_limits<long>::max() - 1));
            if ( divisor <= dividend)  // If divisor is greater than dividend, then the bit is a 1 
            {       	
                dividend -= divisor;     // Subtract divisor from dividend 
                divisor  >>= 1;          // Right shift divisor 
                quotient <<= 1;          // Left shift quotient
                quotient++;              // Increment quotient           // Increment quotient 
            }
            else
            {      	
                divisor >>= 1;             // Bit is 0, right shift divisor, left shift quotient
                quotient <<= 1;
            }
        }
    }

    BigInt::Rossi remainder (dividend); 
    remainder.smartTruncateUnits ();	

    if (o_remainder != NULL) 
    {
        *o_remainder = remainder;
        o_remainder->smartTruncateUnits ();
    }

    quotient.smartTruncateUnits ();

    ASSERTION ((quotient * i_divisor + remainder) == i_dividend);
    return quotient;
}

// ------------------------------------
BigInt::Rossi BigInt::Rossi::naive_pow_iterative(const BigInt::Rossi& i_base, const BigInt::Rossi& i_exp) // Returns base raised to the power of exp
{
	if ((i_base == RossiZero) && (i_exp == RossiZero))
	{
		// In this case we are using std::pow() to be consistent with the system behavior 
		return BigInt::Rossi (static_cast<Unit>(std::pow (static_cast<double>(0), static_cast<double>(0))));
	}

	if (i_exp == RossiZero)
	{
		ASSERTION (i_base != RossiZero);
		return RossiOne;
	}

	if ((i_base == RossiZero) || (i_base == RossiOne))
	{
		return i_base;
	}

	if (i_exp == RossiOne)
	{
		return i_base;
	}

	ASSERTION (i_exp > RossiOne);

	// ------------------
	
	BigInt::Rossi result = RossiOne; 
	
	BigInt::Rossi exp2 = i_exp;
	bool criterion = true;

#ifndef NO_TRY_CATCH
	try
    {
#endif // NO_TRY_CATCH
		while (criterion)
		{
			BigInt::Rossi curResult = i_base;
			BigInt::Rossi exp1 = RossiOne;
			while (true)
			{
				ASSERTION (exp1 <= exp2);
				if ((RossiTwo * exp1) <= exp2)
				{
					curResult = curResult * curResult;
					exp1 += exp1;

					if (exp1 == exp2)
					{
						result = result * curResult;
						criterion = false;
						break;
					}
				}
				else
				{
					ASSERTION (exp1 <= exp2);
					exp2 = exp2 - exp1;
					result = result * curResult;
					if (exp2 == RossiZero)
					{
						criterion = false;
					}
					break;
				}

			} // while-2

		} // while-1
#ifndef NO_TRY_CATCH
	}
	catch (std::exception& i_e)
    {
		std::ostringstream ossFatal;
        ossFatal    << ""
                    << "EXCEPTION ===> "
                    << i_e.what() 
                    << std::endl;
        FATAL_MSG (std::cerr, ossFatal.str());
    }
    catch(...)
    {
		std::ostringstream ossFatal;
        ossFatal    << ""
                    << "UNKNOWN EXCEPTION"
                    << std::endl;
        FATAL_MSG (std::cerr, ossFatal.str());
    }
#endif // NO_TRY_CATCH

	return result;
}

// ------------------------------------
BigInt::Rossi BigInt::Rossi::naive_pow_recursive(const BigInt::Rossi& i_base, const BigInt::Rossi& i_exp) // Returns base raised to the power of exp
{
	if ((i_base == RossiZero) && (i_exp == RossiZero))
	{
		// In this case we are using std::pow() to be consistent with the system behavior 
		return BigInt::Rossi (static_cast<Unit>(std::pow (static_cast<double>(0), static_cast<double>(0))));
	}

	if (i_exp == RossiZero)
	{
		ASSERTION (i_base != RossiZero);
		return RossiOne;
	}

	if ((i_base == RossiZero) || (i_base == RossiOne))
	{
		return i_base;
	}

	if (i_exp == RossiOne)
	{
		return i_base;
	}

	ASSERTION (i_exp > RossiOne);

	// ------------------
	BigInt::Rossi curResult = i_base;
	BigInt::Rossi exp1 = RossiOne;

#ifndef NO_TRY_CATCH
	try
    {
#endif // NO_TRY_CATCH)
		while (true)
		{
			if ((RossiTwo * exp1) <= i_exp)
			{
				curResult = curResult * curResult;
				exp1 = exp1 + exp1;

				if (exp1 == i_exp)
				{
					return curResult;
				}
			}
			else
			{
				return curResult * naive_pow_recursive(i_base, const_cast<BigInt::Rossi&>(i_exp) - exp1); 
			}

		}
#ifndef NO_TRY_CATCH
	}
	catch (std::exception& i_e)
    {
		std::ostringstream ossFatal;
        ossFatal    << ""
                    << "EXCEPTION ===> "
                    << i_e.what() 
                    << std::endl;
        FATAL_MSG (std::cerr, ossFatal.str());
    }
    catch(...)
    {
		std::ostringstream ossFatal;
        ossFatal    << ""
                    << "UNKNOWN EXCEPTION"
                    << std::endl;
        FATAL_MSG (std::cerr, ossFatal.str());
    }
#endif // NO_TRY_CATCH

	ASSERTION(0);  // We may not reach here
	return RossiZero;
}

// ------------------------------------
BigInt::Rossi BigInt::Rossi::pow(const BigInt::Rossi& i_base, const BigInt::Rossi& i_exp,  const bool i_iterative) // Returns base raised to the power of exp
{
	return (i_iterative ? naive_pow_iterative (i_base, i_exp) : naive_pow_recursive (i_base, i_exp));
}

// ------------------------------------
BigInt::Rossi BigInt::Rossi::pow(const BigInt::Rossi& i_base, const Unit i_exp,  const bool i_iterative) // Returns base raised to the power of exp
{
	const BigInt::Rossi exp (i_exp);

	return pow (i_base, exp, i_iterative);
}



// ------------------------------------
BigInt::Rossi BigInt::Rossi::naive_sqrt()		// Returns the square root of this
{

	if (*this == RossiZero)
	{
		return RossiZero;
	}

	if (*this == RossiOne)
	{
		return RossiOne;
	}

	if (*this == RossiTwo)
	{
		return RossiOne;
	}


	// -----------------------

	
	BigInt::Rossi result;

	const long double doubleValue = toDouble();

	const BigInt::Rossi reverseRossi = fromDouble(doubleValue); 
	// ASSERTION (m_units.size() == reverseRossi.m_units.size());
	// ASSERTION (*this == reverseRossi);

	const long double doubleSquare = std::sqrt(doubleValue);

	BigInt::Rossi reverse = fromDouble(doubleSquare);
	BigInt::Rossi curCandidate = reverse;

#ifndef NO_TRY_CATCH
	try
    {
#endif // NO_TRY_CATCH
		while (true)
		{
			if ((curCandidate * curCandidate) <= *this)
			{
				break;
			}
			curCandidate = curCandidate / RossiTwo;
		}


		// -----------------------------------
		BigInt::Rossi minValue = BigInt::Rossi(0); 
		BigInt::Rossi maxValue = BigInt::Rossi(0); 
		const Unit startFactor = 4096 * 4096;
		for (Unit factor = startFactor; factor > 1; factor/=2)
		{
			const BigInt::Rossi theFactor (factor);
			BigInt::Rossi delta (1); 
			// -----------------------
			while (true)
			{
				if (!((curCandidate * curCandidate) <= *this))
				{
					break;
				}
				delta = delta * theFactor;	
				curCandidate = curCandidate + delta;
			}
			ASSERTION ((curCandidate * curCandidate) > *this)
			maxValue = curCandidate;
			curCandidate = curCandidate - delta;
			minValue = curCandidate;
			ASSERTION ((curCandidate * curCandidate) <= *this);
		}

		// ----------------------------------
		ASSERTION (minValue < maxValue);
		ASSERTION (minValue > BigInt::Rossi(0));
		ASSERTION (maxValue > BigInt::Rossi(0));


		const BigInt::Rossi threshold (4);
		while ((maxValue - minValue) >= threshold)
		{
			const BigInt::Rossi mid = minValue + (maxValue - minValue)/RossiTwo;
			if ((mid * mid) > *this)
			{
				maxValue = mid;
			}
			else
			{
				minValue = mid;
			}
		}
		curCandidate = minValue;

		ASSERTION ((curCandidate * curCandidate) <= *this);
	

		// ----------------------------------
		while ((curCandidate * curCandidate) <= *this)
		{
			curCandidate++;
		}
		ASSERTION ((curCandidate * curCandidate) > *this)
		curCandidate--;
		ASSERTION ((curCandidate * curCandidate) <= *this);

#ifndef NO_TRY_CATCH
	 }
     catch (std::exception& i_e)
     {
        std::ostringstream ossFatal;
        ossFatal    << ""
                    << "EXCEPTION ===> "
                    << i_e.what() 
                    << std::endl;
        FATAL_MSG (std::cerr, ossFatal.str());
     }
     catch(...)
     {
        std::ostringstream ossFatal;
        ossFatal    << ""
                    << "UNKNOWN EXCEPTION"
                    << std::endl;
        FATAL_MSG (std::cerr, ossFatal.str());
          
     }
#endif // NO_TRY_CATCH
	return curCandidate;
}


// ------------------
BigInt::Rossi BigInt::Rossi::sqrt()		// Returns the square root of this
{
	return this->naive_sqrt();
}




// ------------------
std::string BigInt::Rossi::toStrHex (const std::string& i_prefix) const
{
    const std::size_t HEX_SETW = sizeof(BigInt::Unit) * 2;
    std::ostringstream oss;

    if (m_units.empty()) 
    {
        oss << i_prefix
            << std::hex
            << static_cast<BigInt::Unit>(0)
            << std::dec;

        return oss.str();
    }

    ASSERTION (!m_units.empty());

    // --------------

    oss << i_prefix
        << std::hex
        << m_units.back();
    for (std::size_t i1 = (m_units.size() - 1); i1; i1--)
    {
        const std::size_t i = i1 - 1;
        oss << std::setw (HEX_SETW) 
            << std::setfill ('0') 
            << m_units [i];
    }
    oss << std::dec;
    return oss.str();
}


// ------------------
std::string BigInt::Rossi::toStr0xHex () const
{
    return toStrHex("0x");
}


// ------------------
std::string BigInt::Rossi::toStrDec (const std::string& i_prefix) const
{
    std::ostringstream oss;

    BigInt::Vin vin (toStrHex(), BigInt::HEX_DIGIT);

    oss << i_prefix
		<< vin.toStrDec();

    return oss.str();
}


// ------------------
std::size_t BigInt::Rossi::getActualHexDigits () const
{
    const std::string str (toStrHex());

	return str.size();
}

// ------------------
std::size_t BigInt::Rossi::getActualDecDigits () const
{
	const std::string str (toStrDec());

	return str.size(); 
}



// ==========================================
void BigInt::Rossi::showUnits(std::ostream& o_stream) const
{
    std::ostringstream oss;

    oss << std::endl;
    oss << ""
        << "BigInt::Rossi value"
        << ": "
        << this->toStr0xHex()      
        << " = "
        << this->toStrDec()
        << std::endl;


    BigInt::BaseBigInt::showUnits(oss);

    oss << std::endl;

    o_stream << std::flush << oss.str() << std::flush;
}



// ------------------
std::ostream& operator<< (std::ostream& o_os, const BigInt::Rossi& i_ins)
{
    // return o_os << i_ins.toStrDec();
    return o_os << i_ins.toStr0xHex();
}


// ------------------
void BigInt::TestVin::testMaxUnits(std::ostream& o_stream)
{
    std::ostringstream oss;

    SET_START_TEST_NAME(oss);

    BigInt::Vin bigInt = VinZero;
    bigInt.maximize();

    oss << "[maximize] Max BigInt::Vin has "
        << bigInt.getUnitsSize()
        << " units"
        << std::endl;

	SET_FINISH_TEST_NAME(oss);

    // ------------------
    o_stream<< std::flush << oss.str() << std::flush;

}

// ------------------
void BigInt::TestRossi::testMaxUnits(std::ostream& o_stream)
{
    std::ostringstream oss;

    SET_START_TEST_NAME(oss);

    BigInt::Rossi bigInt = RossiZero;
    bigInt.maximize();

    oss << "[maximize] Max BigInt::Rossi has "
        << bigInt.getUnitsSize()
        << " units"
        << std::endl;

	SET_FINISH_TEST_NAME(oss);

    // ------------------
    o_stream << std::flush << oss.str() << std::flush;

}



// ------------------
void BigInt::PerformanceTestRossi::performanceTestMaxUnits(std::ostream& o_stream)
{
	SET_TBD(o_stream);

	std::ostringstream oss;

    SET_START_TEST_NAME(oss);

	SET_FINISH_TEST_NAME(oss);

	o_stream << std::flush << oss.str() << std::flush;

}



// ------------------
void BigInt::TestRossi::testMaxMultiplication(std::ostream& o_stream)
{
    std::ostringstream oss;

    SET_START_TEST_NAME(oss);

    BigInt::Rossi RossiTen (BigInt::toString(std::numeric_limits<BigInt::Unit>::max()), BigInt::DEC_DIGIT);
    RossiTen = RossiTen + RossiOne;

    // -----------------------------
    BigInt::Rossi bigInt = RossiOne;
    while (true)
    {
        try
        {
            bigInt = bigInt * RossiTen;
        }
        catch(...)
        {
            // Do nothing
			SET_FINISH_TEST_NAME(oss);
            break;
        }

    }

    oss << "[multiplication] Max BigInt::Rossi has "
        << bigInt.getUnitsSize()
        << " units"
        << std::endl;


	SET_FINISH_TEST_NAME(oss);

    // ------------------
    o_stream << std::flush << oss.str() << std::flush;

}



// ------------------
void BigInt::PerformanceTestRossi::performanceTestMaxMultiplication(std::ostream& o_stream)
{
	SET_TBD(o_stream);

    std::ostringstream oss;

    SET_START_TEST_NAME(oss);

    
	SET_FINISH_TEST_NAME(oss);

    // ------------------
    o_stream << std::flush << oss.str() << std::flush;

}


// ------------------
void BigInt::TestRossi::testTryCatch(std::ostream& o_stream)
{
    std::ostringstream oss;

    oss << ""
        << std::endl
        << std::endl;

    SET_START_TEST_NAME(oss);

    o_stream << std::flush << oss.str() << std::flush;
    oss.str("");

    // -------------------------------
    const BigInt::Rossi rossi(std::numeric_limits<std::size_t>::max(), 0, 1, " - It is OK, it is a stress test");
    // -------------------------------

	SET_FINISH_TEST_NAME(oss);

    // ------------------
    o_stream << std::flush << oss.str() << std::flush;

}




// ------------------
void BigInt::TestRossi::testDouble(std::ostream& o_stream, const BigInt::Rossi& i_bigIntValue, const std::size_t i_serialNo)
{
	std::ostringstream oss;

	const long double toDoubleValue = i_bigIntValue.toDouble();
	const BigInt::Rossi afterFromDoubleValue = BigInt::Rossi::fromDouble(toDoubleValue);

	const std::string s1 ((i_bigIntValue == afterFromDoubleValue) ? "+" : "-");

	if (i_serialNo != std::numeric_limits<std::size_t>::max())
	{
		oss << ""
			<< "(#"
			<< i_serialNo
			<< ") ";
	}

	oss << ""
		<< "["
		<< s1
		<< "] "
		<< "bigInt: " 
		<< "Hex = "
		<< i_bigIntValue
		<< ", "
		<< "Dec = "
		<< i_bigIntValue.toStrDec()
		<< ", decimalDigits = "
		<< i_bigIntValue.toStrDec().size()
		<< " ---> toDouble = "
		<< std::setprecision (std::numeric_limits<long double>::digits10)
		<< toDoubleValue
		<< " ---> BigIntAfterFromDouble: "
		<< "Hex = "
		<< afterFromDoubleValue
		<< ", "
		<< "Dec = "
		<< afterFromDoubleValue.toStrDec()
		<< std::endl;

	o_stream << oss.str() << std::endl;
}





// ------------------
void BigInt::TestRossi::testAssertDouble(std::ostream& o_stream, const BigInt::Unit i_ulongValue, std::size_t& io_count)
{

	std::ostringstream oss;

	const long double toDoubleUnitValue = static_cast<long double>(i_ulongValue);
	const Unit afterFromDoubleUnitValue = static_cast<Unit> (toDoubleUnitValue);

	std::ostringstream oss1;
	oss1 << std::hex << i_ulongValue << std::dec;
	const std::size_t hexDigitsInResultUnit = oss1.str().size();
	const std::size_t bitsInResultUnit = hexDigitsInResultUnit * CHAR_BIT;

	if (bitsInResultUnit < std::numeric_limits<double>::digits)
	{
	   io_count++;
	   ASSERTION (i_ulongValue == afterFromDoubleUnitValue);
	}
	
	
	
	BigInt::Rossi bigIntValue (i_ulongValue);

	const long double toDoubleBigIntValue = bigIntValue.toDouble();
	const BigInt::Rossi afterFromDoubleBigIntValue = BigInt::Rossi::fromDouble(toDoubleBigIntValue);

	BigInt::Rossi bigIntValueAfter (afterFromDoubleUnitValue);


    if (bigIntValueAfter.getUnitsSize() == 1)
	{
		std::ostringstream oss2;
		oss2 << std::hex << bigIntValueAfter << std::dec;
		const std::size_t hexDigitsInResultUnit = oss2.str().size();
		const std::size_t bitsInResultUnit = hexDigitsInResultUnit * CHAR_BIT;

		if (bitsInResultUnit < std::numeric_limits<double>::digits)
		{
		   io_count++;
		   ASSERTION (bigIntValue == bigIntValueAfter);
		}
	}	

	o_stream << oss.str();
}

// ------------------
void BigInt::TestRossi::testPrintAll(std::ostream& o_stream)
{
	SET_START_TEST_NAME(o_stream);

	std::ostringstream oss;

	std::string strValue ("1");

	const std::size_t delta = 5;

	for (std::size_t i = 0; i < 100; i++)
	{
		strValue += "0";

		BigInt::Rossi rossi1 (strValue.c_str(), BigInt::DEC_DIGIT);
		rossi1 -= BigInt::Rossi (delta);

		for (std::size_t k = 0; k < (2 *delta); k++)
		{
				rossi1 += RossiOne;

				const std::size_t rossiBigIntSize = rossi1.getUnitsSize();
				const BigInt::Vin vin (rossi1);
				const std::size_t vinBigIntSize = vin.getUnitsSize();

				oss << ""
					<< "[Dec] "
					<< "Value = "
					<< rossi1.toStrDec() 
					<< " ===> "
					<< " VinUnits = "
					<< vinBigIntSize
					<< ", RossiUnits = "
					<< rossiBigIntSize
					<< std::endl;
		}

		oss << std::endl;

		// ------------------
		BigInt::Rossi rossi2 (strValue.c_str(), BigInt::HEX_DIGIT);
		rossi2 -= BigInt::Rossi (delta);

		for (std::size_t k = 0; k < (2 *delta); k++)
		{
				rossi2 += RossiOne;

				const std::size_t rossiBigIntSize = rossi1.getUnitsSize();
				const BigInt::Vin vin (rossi2);
				const std::size_t vinBigIntSize = vin.getUnitsSize();

				oss << ""
					<< "[Hex] "
					<< "Value = "
					<< rossi2.toStr0xHex()
					<< " ===> "
					<< " VinUnits = "
					<< vinBigIntSize
					<< ", RossiUnits = "
					<< rossiBigIntSize
					<< std::endl;
		}

		oss << std::endl;
		oss << std::endl;
	}
	
	o_stream << oss.str();

	SET_FINISH_TEST_NAME(o_stream);
}

// ------------------
void BigInt::TestRossi::testAssertPrintAll(std::ostream& o_stream)
{
	SET_START_TEST_NAME(o_stream);

	std::string strValue ("1");

	const std::size_t delta = 5;

	const std::size_t workPrintInterval = 100;

	std::size_t count = 0;
	for (std::size_t i = 0; i < 500; i++)
	{
		if ((i % workPrintInterval) == 0)
		{
			SET2_WORKING_TEST_NAME(std::cerr, count++);
		}


		strValue += "0";

		BigInt::Rossi rossi1 (strValue.c_str(), BigInt::DEC_DIGIT);
		rossi1 -= BigInt::Rossi (delta);

		for (std::size_t k = 0; k < (2 *delta); k++)
		{
				std::ostringstream oss;

				BigInt::Rossi rossiPrevValue = rossi1;
				rossi1 += RossiOne;
				
				ASSERTION (rossi1 == (rossiPrevValue + RossiOne));

				oss << rossi1.toStrDec();

				const BigInt::Rossi rossiPrintValue (oss.str().c_str(), BigInt::DEC_DIGIT);
				ASSERTION (rossi1 == rossiPrintValue);
				
		}

		// ------------------
		BigInt::Rossi rossi2 (strValue.c_str(), BigInt::HEX_DIGIT);
		rossi2 -= BigInt::Rossi (delta);

		for (std::size_t k = 0; k < (2 *delta); k++)
		{
				std::ostringstream oss;

				BigInt::Rossi rossiPrevValue = rossi2;
				rossi2 += RossiOne;

				ASSERTION (rossi2 == (rossiPrevValue + RossiOne));

				oss << rossi2.toStrHex();

				const BigInt::Rossi rossiPrintValue (oss.str().c_str(), BigInt::HEX_DIGIT);
				ASSERTION (rossi2 == rossiPrintValue);
		}

	}
	
	SET_FINISH_TEST_NAME(o_stream);
}



// ------------------
void BigInt::PerformanceTestRossi::performanceTestDouble(std::ostream& o_stream)
{
	SET_TBD(o_stream);

	SET_START_TEST_NAME(o_stream);

	SET_FINISH_TEST_NAME(o_stream);
}



// ------------------
void BigInt::TestRossi::testDoubleAll(std::ostream& o_stream)
{
	SET_START_TEST_NAME(o_stream);

	std::string str ("");
	std::size_t k = 0;
	for (int i = 0; i < 30; i++)
	{
	  str += "f";

	  BigInt::Rossi bigInt3(str, BigInt::HEX_DIGIT);
	  BigInt::Rossi bigInt1 = bigInt3 - BigInt::Rossi(1);
	  BigInt::Rossi bigInt2 = bigInt1 - BigInt::Rossi(1);
	  BigInt::Rossi bigInt4 = bigInt3 + BigInt::Rossi(1);
	  BigInt::Rossi bigInt5 = bigInt4 + BigInt::Rossi(1);

	  testDouble (o_stream, bigInt1, k++);
	  testDouble (o_stream, bigInt2, k++);
	  testDouble (o_stream, bigInt3, k++);
	  testDouble (o_stream, bigInt4, k++);
	  testDouble (o_stream, bigInt5, k++);
	}

	SET_FINISH_TEST_NAME(o_stream);
}



// ------------------
void BigInt::TestRossi::testAssertDoubleAll(std::ostream& o_stream)
{
	SET_START_TEST_NAME(o_stream);

	std::size_t count = 0;
	std::size_t localTestCount = 0;

	SET2_WORKING_TEST_NAME(std::cerr, count++);
	for (Unit i = 0; i < 1030; i++)
	{
	  testAssertDouble (o_stream, i, localTestCount);
	}

	SET2_WORKING_TEST_NAME(std::cerr, count++);

	const Unit delta = 0xfffffe;
	const Unit upperBound = 0xffffffff;
	for (Unit i = 0; i < upperBound; )
	{
	  SET2_WORKING_TEST_NAME(std::cerr, count++);
	  testAssertDouble (o_stream, i, localTestCount);
	  SET2_WORKING_TEST_NAME(std::cerr, count++);

	  if (
		     (static_cast<long double>(i) + static_cast<long double>(delta)) 
			  >= 
			  static_cast<long double>(upperBound)
		 )
	  {
		  break;
	  }

	   i += delta; 
	}

	SET2_WORKING_TEST_NAME(std::cerr, count++);
	for (Unit i = std::numeric_limits<Unit>::max() - 5; i < std::numeric_limits<Unit>::max(); i++)
	{
	  SET2_WORKING_TEST_NAME(std::cerr, count++);
	  testAssertDouble (o_stream, i, localTestCount);
	  SET2_WORKING_TEST_NAME(std::cerr, count++);
	}

	ASSERTION (localTestCount > 0);

	SET2_WORKING_TEST_NAME(std::cerr, count++);

	SET_FINISH_TEST_NAME(o_stream);
}


// ------------------
void BigInt::PerformanceTestRossi::performanceTestPow(std::ostream& o_stream)
{
	
	SET_START_TEST_NAME(o_stream);

	std::ostringstream ossBase;
	std::ostringstream ossExp;
	std::ostringstream ossDelta;

	ossBase << "abcdef1234983ba5";
	ossExp << "135";
	ossDelta << std::string (50, 'a');

	std::size_t runs = 5;

	std::size_t repetitions = 1;

	std::size_t count = 0;

	for (std::size_t k = 0; k < runs; k++)
	{
		SET3_WORKING_TEST_NAME(std::cerr, k + 1, runs);

		Rossi base (ossBase.str().c_str(), HEX_DIGIT);
		Rossi exp (ossExp.str().c_str(), HEX_DIGIT);
		Rossi result (0);

		START_TIME;

		for (std::size_t i = 0; i < repetitions; i++)
		{
			result = BigInt::Rossi::pow (base, exp);
			s_returnValue+= result.getUnitsSize();
		}

		END_TIME;

		if (s_elapsedTime < s_clockMinThreshold)
		{
			repetitions *= 10;
			runs++;
			continue;
		}

		DISPLAY_TIME(count++, std::cout, base, &result,  repetitions, "Rossi::pow()", s_elapsedTime);

		repetitions = 1;

		ossBase << ossDelta.str();
		
	}

	SET_FINISH_TEST_NAME(o_stream);
}



// ------------------
void BigInt::TestRossi::testPow(std::ostream& o_stream, const BigInt::Rossi& i_base, const BigInt::Rossi& i_exp)
{
	std::ostringstream oss;

	const BigInt::Rossi resultIterative = BigInt::Rossi::pow (i_base, i_exp);
	const BigInt::Rossi resultRecursive = BigInt::Rossi::pow (i_base, i_exp, false);
	ASSERTION(resultIterative == resultRecursive);

	oss << ""
		<< "[Hex] "
		<< "pow ("
		<< i_base 
		<< ", " 
		<< i_exp 
		<< ") = " 
		<< resultIterative 
		<< std::endl;

	oss << ""
		<< "[Dec] "
		<< "pow ("
		<< i_base.toStrDec() 
		<< ", " 
		<< i_exp.toStrDec() 
		<< ") = "  
		<< resultIterative.toStrDec() 
		<< std::endl;

	o_stream << oss.str() << std::endl;
}





// ------------------
void BigInt::TestRossi::testAssertPow(std::ostream& o_stream, const BigInt::Unit i_base, const BigInt::Unit i_exp, std::size_t& io_count)
{

	std::ostringstream oss;
	const BigInt::Rossi base(i_base);
	const BigInt::Rossi exp(i_exp);

	const Unit resultUnit = static_cast<Unit>(std::pow(static_cast<double>(i_base), static_cast<double>(i_exp)));

	const BigInt::Rossi resultBigIntIterative = BigInt::Rossi::pow (base, exp);
	const BigInt::Rossi resultBigIntRecursive = BigInt::Rossi::pow (base, exp, false);
	ASSERTION(resultBigIntIterative == resultBigIntRecursive);

	const BigInt::Rossi resultViaUnit (resultUnit);

	if (resultBigIntIterative.getUnitsSize() == 1)
	{
		std::ostringstream oss1;
		oss1 << std::hex << resultUnit << std::dec;
		const std::size_t hexDigitsInResultUnit = oss1.str().size();
		const std::size_t bitsInResultUnit = hexDigitsInResultUnit * CHAR_BIT;

		if (bitsInResultUnit < std::numeric_limits<double>::digits)
		{
			io_count++;
			ASSERTION(resultViaUnit == resultBigIntIterative);
		}
	}

	o_stream << oss.str();
}




// ------------------
void BigInt::TestRossi::testPow(std::ostream& o_stream, const BigInt::Rossi& i_base, BigInt::Unit i_exp)
{
	std::ostringstream oss;

	const BigInt::Rossi resultIterative = BigInt::Rossi::pow (i_base, i_exp);
	const BigInt::Rossi resultRecursive = BigInt::Rossi::pow (i_base, i_exp, false);
	ASSERTION(resultIterative == resultRecursive);

	oss << ""
		<< "[Hex] "
		<< "pow ("
		<< i_base 
		<< ", " 
		<< std::hex
		<< std::showbase
		<< i_exp 
		<< std::dec
		<< ") = " 
		<< resultIterative 
		<< std::endl;

	oss << ""
		<< "[Dec] "
		<< "pow ("
		<< i_base.toStrDec() 
		<< ", "
		<< i_exp
		<< ") = "  
		<< resultIterative.toStrDec() 
		<< std::endl;

	o_stream << oss.str() << std::endl;
}





// -------------------------------------
void BigInt::TestRossi::testPowAll(std::ostream& o_stream)
{
	SET_START_TEST_NAME(o_stream);

	std::size_t count = 0;
	for (BigInt::Unit p = 0; p < 23; p++)
	{
		for (BigInt::Rossi k = RossiZero; k < BigInt::Rossi(1003); k++)
		{
			testPow (o_stream, k, p); 
		}
	}

	SET2_WORKING_TEST_NAME(std::cerr, count++);

	for (BigInt::Unit p = 0; p < 105; p++)
	{
		
		for (BigInt::Rossi k = RossiZero; k < BigInt::Rossi(111); k++)
		{
			testPow (o_stream, k, p); 
		}
	}

	SET2_WORKING_TEST_NAME(std::cerr, count++);

	for (BigInt::Rossi p = RossiZero; p < BigInt::Rossi (21); p++)
	{
		for (BigInt::Rossi k = RossiZero; k < BigInt::Rossi(1001); k++)
		{
			testPow (o_stream, k, p); 
		}
	}

	SET2_WORKING_TEST_NAME(std::cerr, count++);
	for (BigInt::Rossi p = RossiZero; p < BigInt::Rossi (107); p++)
	{
		for (BigInt::Rossi k = RossiZero; k < BigInt::Rossi(115); k++)
		{
			testPow (o_stream, k, p); 
		}
	}

	SET_FINISH_TEST_NAME(o_stream);
}


// -------------------------------------
void BigInt::TestRossi::testAssertPowAll(std::ostream& o_stream)
{
	SET_START_TEST_NAME(o_stream);

	std::size_t count = 0;
	std::size_t localTestCount = 0;

	SET2_WORKING_TEST_NAME(std::cerr, count++);
	for (BigInt::Unit p = 0; p < 23; p++)
	{
		for (BigInt::Unit k = 0; k < 1003; k++)
		{
			testAssertPow (o_stream, k, p, localTestCount); 
		}
	}

	SET2_WORKING_TEST_NAME(std::cerr, count++);
	for (BigInt::Unit p = 0; p < 105; p++)
	{
		for (BigInt::Unit k = 0; k < 111; k++)
		{
			testAssertPow (o_stream, k, p, localTestCount); 
		}
	}

	SET2_WORKING_TEST_NAME(std::cerr, count++);
	for (Unit p = 0; p < 21; p++)
	{
		for (BigInt::Unit k = 0; k < 1001; k++)
		{
			testAssertPow (o_stream, k, p, localTestCount); 
		}
	}

	SET2_WORKING_TEST_NAME(std::cerr, count++);
	for (Unit p = 0; p < 107; p++)
	{
		for (BigInt::Unit k = 0; k < 115; k++)
		{
			testAssertPow (o_stream, k, p, localTestCount); 
		}
	}


	ASSERTION (localTestCount > 0);

	SET2_WORKING_TEST_NAME(std::cerr, count++);

	SET_FINISH_TEST_NAME(o_stream);
}





// ------------------
void BigInt::PerformanceTestRossi::performanceTestSqrt(std::ostream& o_stream)
{

	SET_START_TEST_NAME(o_stream);

	std::ostringstream ossSource;
	std::ostringstream ossDelta1;
	std::ostringstream ossDelta2;

	ossSource << "abcdef1234";
	ossDelta1 << "1234567890123456789123456789";
	ossDelta2 << std::string (300, 'a');

	const std::size_t preRunsThreshold = 2;

	std::size_t runs = 10;

	std::size_t repetitions = 1;

	std::size_t count = 0;
	for (std::size_t k = 0; k < runs; k++)
	{
		SET3_WORKING_TEST_NAME(std::cerr, k + 1, runs);

		Rossi rossi (ossSource.str().c_str(), HEX_DIGIT);
		Rossi result (0);

		START_TIME;

		for (std::size_t i = 0; i < repetitions; i++)
		{
			result = rossi.sqrt();
			s_returnValue+= result.getUnitsSize();
		}

		END_TIME;

		if (s_elapsedTime < s_clockMinThreshold)
		{
			repetitions *= 10;
			runs++;
			continue;
		}

		DISPLAY_TIME(count++, std::cout, rossi, static_cast<Rossi*>(0),  repetitions, "Rossi::sqrt()", s_elapsedTime);

		repetitions = 1;

		if (k < (runs - preRunsThreshold))
		{
			ossSource << ossDelta1.str();
		}
		else
		{
			ossSource << ossDelta2.str();
		}
	}

	SET_FINISH_TEST_NAME(o_stream);

}


// ------------------
void BigInt::TestRossi::testSqrt(std::ostream& o_stream)
{
    SET_START_TEST_NAME(o_stream);

	std::ostringstream oss;

    BigInt::Rossi result;

	std::size_t count = 0;
    // ------------------------
	SET2_WORKING_TEST_NAME(std::cerr, count++);
	for (BigInt::Unit i = 0; i < 1025; i++)
	{
       BigInt::Rossi arg1(i);
	
	   result = arg1.sqrt();
	   oss << "[Hex] sqrt(" << arg1 << ") " << " = " << result << std::endl;
	   oss << "[Dec] sqrt(" << arg1.toStrDec() << ") " << " = " << result.toStrDec() << std::endl;
	   oss << std::endl;
    }

	SET2_WORKING_TEST_NAME(std::cerr, count++);
    const BigInt::Unit delta = 10;
	for (BigInt::Unit i = std::numeric_limits<BigInt::Unit>::max() - delta; i < std::numeric_limits<BigInt::Unit>::max(); i++)
	{
       BigInt::Rossi arg1(i);
	
	   result = arg1.sqrt();
	   oss << "[Hex] sqrt(" << arg1 << ") " << " = " << result << std::endl;
	   oss << "[Dec] sqrt(" << arg1.toStrDec() << ") " << " = " << result.toStrDec() << std::endl;
	   oss << std::endl;
    }

	SET2_WORKING_TEST_NAME(std::cerr, count++);
	BigInt::Rossi arg2 ("99999999999999999999999999999999", BigInt::DEC_DIGIT);
	for (std::size_t i = 0; i < 5; i++, arg2++)
	{
	   SET2_WORKING_TEST_NAME(std::cerr, count++);
	   result = arg2.sqrt();
	   SET2_WORKING_TEST_NAME(std::cerr, count++);

	   oss << "[Hex] sqrt(" << arg2 << ") " << " = " << result << std::endl;
	   oss << "[Dec] sqrt(" << arg2.toStrDec() << ") " << " = " << result.toStrDec() << std::endl;
	   oss << std::endl;

    }

	SET2_WORKING_TEST_NAME(std::cerr, count++);
	BigInt::Rossi arg3 ("99999999999999999999999999999999", BigInt::DEC_DIGIT);
	for (std::size_t i = 0; i < 4; i++)
	{
	   SET2_WORKING_TEST_NAME(std::cerr, count++);
	   result = arg3.sqrt();
	   SET2_WORKING_TEST_NAME(std::cerr, count++);

	   oss << "[Hex] sqrt(" << arg3 << ") " << " = " << result << std::endl;
	   oss << "[Dec] sqrt(" << arg3.toStrDec() << ") " << " = " << result.toStrDec() << std::endl;
	   oss << std::endl;

	   arg3 = arg3 * arg3;

    }

	SET2_WORKING_TEST_NAME(std::cerr, count++);

	o_stream << oss.str();
	SET_FINISH_TEST_NAME(o_stream);

}

// ------------------
void BigInt::TestRossi::testAssertSqrt(std::ostream& o_stream)
{
    SET_START_TEST_NAME(o_stream);

	std::ostringstream oss;

    BigInt::Rossi resultRossi = RossiZero;
	Unit resultUnit = 0;

	BigInt::Rossi resultRossiViaUnit = RossiZero;
	Unit resultUnitViaRossi = 0;

	std::size_t count = 0;
	std::size_t localTestCount = 0;
    // ------------------------
	SET2_WORKING_TEST_NAME(std::cerr, count++);
	for (BigInt::Unit i = 0; i < 1025; i++)
	{
       BigInt::Rossi arg1(i);
	
	   resultRossi = arg1.sqrt();
	   resultUnit = static_cast<Unit>(std::sqrt(static_cast<long double>(i)));

	   ASSERTION (resultRossi.getUnitsSize() == 1);

	   resultRossiViaUnit = BigInt::Rossi (resultUnit);
	   resultUnitViaRossi = resultRossi.toUnit();
 
	   ASSERTION (resultRossi == resultRossiViaUnit);
	   ASSERTION (resultUnit == resultUnitViaRossi);
    }

	const BigInt::Unit delta = 1000;


	SET2_WORKING_TEST_NAME(std::cerr, count++);

	for (BigInt::Unit i = std::numeric_limits<unsigned int>::max() - delta; i < std::numeric_limits<unsigned int>::max(); i++)
	{
       BigInt::Rossi arg1(i);
	
	   resultRossi = arg1.sqrt();
	   resultUnit = static_cast<Unit>(std::sqrt(static_cast<long double>(i)));

	   ASSERTION (resultRossi.getUnitsSize() == 1);

	   resultRossiViaUnit = BigInt::Rossi (resultUnit);
	   resultUnitViaRossi = resultRossi.toUnit();

	   if (resultRossiViaUnit.getUnitsSize() == 1)
	   {
		  std::ostringstream oss1;
		  oss1 << std::hex << resultUnit << std::dec;
		  const std::size_t hexDigitsInResultUnit = oss1.str().size();
		  const std::size_t bitsInResultUnit = hexDigitsInResultUnit * CHAR_BIT;

		  if (bitsInResultUnit < std::numeric_limits<double>::digits)
		  {
			 localTestCount++;
			 ASSERTION (resultRossi == resultRossiViaUnit);
			 ASSERTION (resultUnit == resultUnitViaRossi);
		  }
	   }

    }


	SET2_WORKING_TEST_NAME(std::cerr, count++);

	for (unsigned long i = std::numeric_limits<unsigned long>::max() - delta; i < std::numeric_limits<unsigned long>::max(); i++)
	{
       BigInt::Rossi arg1(static_cast<BigInt::Unit>(i));
	
	   resultRossi = arg1.sqrt();
	   resultUnit = static_cast<Unit>(std::sqrt(static_cast<long double>(i)));

	   ASSERTION (resultRossi.getUnitsSize() == 1);

	   resultRossiViaUnit = BigInt::Rossi (resultUnit);
	   resultUnitViaRossi = resultRossi.toUnit();

	   if (resultRossiViaUnit.getUnitsSize() == 1)
	   {
		  std::ostringstream oss1;
		  oss1 << std::hex << resultUnit << std::dec;
		  const std::size_t hexDigitsInResultUnit = oss1.str().size();
		  const std::size_t bitsInResultUnit = hexDigitsInResultUnit * CHAR_BIT;

		  if (bitsInResultUnit < std::numeric_limits<double>::digits)
		  {
			 localTestCount++;
			 ASSERTION (resultRossi == resultRossiViaUnit);
			 ASSERTION (resultUnit == resultUnitViaRossi);
		  }
	   }

    }

	SET2_WORKING_TEST_NAME(std::cerr, count++);

	for (BigInt::Unit i = std::numeric_limits<BigInt::Unit>::max() - delta; i < std::numeric_limits<BigInt::Unit>::max(); i++)
	{
       BigInt::Rossi arg1(i);
	
	   resultRossi = arg1.sqrt();
	   resultUnit = static_cast<Unit>(std::sqrt(static_cast<long double>(i)));

	   ASSERTION (resultRossi.getUnitsSize() == 1);

	   resultRossiViaUnit = BigInt::Rossi (resultUnit);
	   resultUnitViaRossi = resultRossi.toUnit();

	   if (resultRossiViaUnit.getUnitsSize() == 1)
	   {
		  std::ostringstream oss1;
		  oss1 << std::hex << resultUnit << std::dec;
		  const std::size_t hexDigitsInResultUnit = oss1.str().size();
		  const std::size_t bitsInResultUnit = hexDigitsInResultUnit * CHAR_BIT;

		  if (bitsInResultUnit < std::numeric_limits<double>::digits)
		  {
			 localTestCount++;
			 ASSERTION (resultRossi == resultRossiViaUnit);
			 ASSERTION (resultUnit == resultUnitViaRossi);
		  }
	   }

    }

	ASSERTION (localTestCount > 0);
	SET2_WORKING_TEST_NAME(std::cerr, count++);

	o_stream << oss.str();
	SET_FINISH_TEST_NAME(o_stream);

}



// ------------------
void BigInt::PerformanceTestRossi::performanceTestOperatorAdd(std::ostream& o_stream)
{

    SET_START_TEST_NAME(o_stream);

	std::ostringstream ossSource;
	std::ostringstream ossDelta1;
	std::ostringstream ossDelta2;

	ossSource << "abcdef1234";
	ossDelta1 << "1234567890123456789123456789";
	ossDelta2 << std::string (2500, 'a');

	const std::size_t preRunsThreshold = 2;

	std::size_t runs = 10;

	std::size_t repetitions = 1;

	std::size_t count = 0;
	for (std::size_t k = 0; k < runs; k++)
	{
		SET3_WORKING_TEST_NAME(std::cerr, k + 1, runs);

		Rossi rossi (ossSource.str().c_str(), HEX_DIGIT);
		Rossi result (0);

		START_TIME;

		for (std::size_t i = 0; i < repetitions; i++)
		{
			result = rossi + result;
			s_returnValue+= result.getUnitsSize();
		}

		END_TIME;

		if (s_elapsedTime < s_clockMinThreshold)
		{
			repetitions *= 10;
			runs++;
			continue;
		}

		DISPLAY_TIME(count++, std::cout, rossi, &result,  repetitions, "Rossi::operator+", s_elapsedTime);

		repetitions = 1;

		if (k < (runs - preRunsThreshold))
		{
			ossSource << ossDelta1.str();
		}
		else
		{
			ossSource << ossDelta2.str();
		}
	}

	SET_FINISH_TEST_NAME(o_stream);


}



// ------------------
void BigInt::TestRossi::testOperatorAdd(std::ostream& o_stream)
{
    SET_START_TEST_NAME(o_stream);

    std::vector<std::pair<BigInt::Rossi, BigInt::Rossi> > vtest = BigInt::TestRossi::fillTestInputPairsRossiRossi();

    // --- test ---
    for (std::size_t i = 0; i < vtest.size(); i++)
    {
        ROSSI_TEST_COMPUTE_BINARY_OP(o_stream, vtest[i], +);
    }

	SET_FINISH_TEST_NAME(o_stream);

}



// ------------------
void BigInt::TestRossi::testAssertOperatorAdd(std::ostream& o_stream)
{

    SET_START_TEST_NAME(o_stream);

    std::vector<std::pair<BigInt::Rossi, BigInt::Rossi> > vtest = BigInt::TestRossi::fillTestInputPairsRossiRossi();

    // --- test ---
	bool emptyTestSet = true;
    for (std::size_t i = 0; i < vtest.size(); i++)
    {
		if (vtest[i].first.getUnitsSize() > 1)
		{
			continue;
		}

		if (vtest[i].second.getUnitsSize() > 1)
		{
			continue;
		}

		BigInt::Rossi rossi1 = vtest[i].first;
		BigInt::Rossi rossi2 = vtest[i].second;
		BigInt::Rossi rossiResult = rossi1 + rossi2;

		if (rossiResult.getUnitsSize() > 1)
		{
			continue;
		}
		emptyTestSet = false;

		const Unit ulong1 = rossi1.toUnit(); 
		const Unit ulong2 = rossi2.toUnit(); 
		const Unit ulongResult = ulong1 + ulong2;

		const BigInt::Rossi rossiResultViaUnit (ulongResult);

		const Unit ulongResultViaRossi = rossiResult.toUnit();

		ASSERTION(rossiResult.getUnitsSize() == rossiResultViaUnit.getUnitsSize());
		if (rossiResult.getUnitsSize() == 1)
		{
			ASSERTION(rossiResultViaUnit.getUnitsSize() == 1);
			ASSERTION(rossiResult == rossiResultViaUnit);
			ASSERTION(ulongResult == ulongResultViaRossi);
		}
    }
	ASSERTION(!emptyTestSet);

	SET_FINISH_TEST_NAME(o_stream);
}




// ------------------
void BigInt::PerformanceTestRossi::performanceTestOperatorAddAssign(std::ostream& o_stream)
{
	SET_TBD(o_stream);

    SET_START_TEST_NAME(o_stream);


    SET_FINISH_TEST_NAME(o_stream);

}



// ------------------
void BigInt::TestRossi::testOperatorAddAssign(std::ostream& o_stream)
{
    SET_START_TEST_NAME(o_stream);

    std::vector<std::pair<BigInt::Rossi, BigInt::Rossi> > vtest = BigInt::TestRossi::fillTestInputPairsRossiRossi();

    // --- test ---
    for (std::size_t i = 0; i < vtest.size(); i++)
    {
        BIGINT_TEST_COMPUTE_UNARY_OP(o_stream, vtest[i], +=);
    }

    SET_FINISH_TEST_NAME(o_stream);

}


// ------------------
void BigInt::TestRossi::testAssertOperatorAddAssign(std::ostream& o_stream)
{

    SET_START_TEST_NAME(o_stream);

   std::vector<std::pair<BigInt::Rossi, BigInt::Rossi> > vtest = BigInt::TestRossi::fillTestInputPairsRossiRossi();

    // --- test ---
	bool emptyTestSet = true;
    for (std::size_t i = 0; i < vtest.size(); i++)
    {
		if (vtest[i].first.getUnitsSize() > 1)
		{
			continue;
		}

		if (vtest[i].second.getUnitsSize() > 1)
		{
			continue;
		}

		BigInt::Rossi rossi1 = vtest[i].first;
		BigInt::Rossi rossi2 = vtest[i].second;

		Unit ulong1 = rossi1.toUnit(); 

		rossi1 += rossi2;

		if (rossi1.getUnitsSize() > 1)
		{
			continue;
		}
		
		const Unit ulong2 = rossi2.toUnit(); 
		ulong1 += ulong2;

		if (rossi1.getUnitsSize() > 1)
		{
			continue;
		}
		emptyTestSet = false;
		
		const BigInt::Rossi rossiResultViaUnit (ulong1);

		const Unit ulongResultViaRossi = rossi1.toUnit();

		ASSERTION(rossi1.getUnitsSize() == rossiResultViaUnit.getUnitsSize());
		if (rossi1.getUnitsSize() == 1)
		{
			ASSERTION(rossiResultViaUnit.getUnitsSize() == 1);
			ASSERTION(rossi1 == rossiResultViaUnit);
			ASSERTION(ulong1 == ulongResultViaRossi);
		}
    }
	ASSERTION(!emptyTestSet);

    SET_FINISH_TEST_NAME(o_stream);
}





// ------------------
void BigInt::PerformanceTestRossi::performanceTestOperatorSubtraction(std::ostream& o_stream)
{
	SET_TBD(o_stream);

    SET_START_TEST_NAME(o_stream);

	SET_FINISH_TEST_NAME(o_stream);

}

// ------------------
void BigInt::TestRossi::testOperatorSubtraction(std::ostream& o_stream)
{
    SET_START_TEST_NAME(o_stream);

    std::vector<std::pair<BigInt::Rossi, BigInt::Rossi> > vtest = BigInt::TestRossi::fillTestInputPairsRossiRossi();
 
    // --- test ---
    for (std::size_t i = 0; i < vtest.size(); i++)
    {
        if (vtest[i].first < vtest[i].second)
        {
            continue;
        }
        ROSSI_TEST_COMPUTE_BINARY_OP(o_stream, vtest[i], -);
    }

	SET_FINISH_TEST_NAME(o_stream);

}


// ------------------
void BigInt::TestRossi::testAssertOperatorSubtraction(std::ostream& o_stream)
{

    SET_START_TEST_NAME(o_stream);

    std::vector<std::pair<BigInt::Rossi, BigInt::Rossi> > vtest = BigInt::TestRossi::fillTestInputPairsRossiRossi();
 
   
    // --- test ---
	bool emptyTestSet = true;
    for (std::size_t i = 0; i < vtest.size(); i++)
    {
		if (vtest[i].first < vtest[i].second)
        {
            continue;
        }

		if (vtest[i].first.getUnitsSize() > 1)
		{
			continue;
		}

		if (vtest[i].second.getUnitsSize() > 1)
		{
			continue;
		}
		emptyTestSet = false;

		BigInt::Rossi rossi1 = vtest[i].first;
		BigInt::Rossi rossi2 = vtest[i].second;
		BigInt::Rossi rossiResult = rossi1 - rossi2;

		const Unit ulong1 = rossi1.toUnit(); 
		const Unit ulong2 = rossi2.toUnit(); 
		const Unit ulongResult = ulong1 - ulong2;

		const BigInt::Rossi rossiResultViaUnit (ulongResult);

		const Unit ulongResultViaRossi = rossiResult.toUnit();

		ASSERTION(rossiResult.getUnitsSize() == rossiResultViaUnit.getUnitsSize());
		if (rossiResult.getUnitsSize() == 1)
		{
			ASSERTION(rossiResultViaUnit.getUnitsSize() == 1);
			ASSERTION(rossiResult == rossiResultViaUnit);
			ASSERTION(ulongResult == ulongResultViaRossi);
		}
    }
	ASSERTION(!emptyTestSet);

	SET_FINISH_TEST_NAME(o_stream);

}






// ------------------
void BigInt::PerformanceTestRossi::performanceTestOperatorSubtractionAssign(std::ostream& o_stream)
{
	SET_TBD(o_stream);

    SET_START_TEST_NAME(o_stream);

	SET_FINISH_TEST_NAME(o_stream);

}


// ------------------
void BigInt::TestRossi::testOperatorSubtractionAssign(std::ostream& o_stream)
{
    SET_START_TEST_NAME(o_stream);

    std::vector<std::pair<BigInt::Rossi, BigInt::Rossi> > vtest = BigInt::TestRossi::fillTestInputPairsRossiRossi();

 
    // --- test ---
    for (std::size_t i = 0; i < vtest.size(); i++)
    {
        if (vtest[i].first < vtest[i].second)
        {
            continue;
        }
        BIGINT_TEST_COMPUTE_UNARY_OP(o_stream, vtest[i], -=);
    }

	SET_FINISH_TEST_NAME(o_stream);

}


// ------------------
void BigInt::TestRossi::testAssertOperatorSubtractionAssign(std::ostream& o_stream)
{
    SET_START_TEST_NAME(o_stream);

    std::vector<std::pair<BigInt::Rossi, BigInt::Rossi> > vtest = BigInt::TestRossi::fillTestInputPairsRossiRossi();

 
    // --- test ---
	bool emptyTestSet = true;
    for (std::size_t i = 0; i < vtest.size(); i++)
    {
		if (vtest[i].first < vtest[i].second)
        {
             continue;
        }  

		if (vtest[i].first.getUnitsSize() > 1)
		{
			continue;
		}

		if (vtest[i].second.getUnitsSize() > 1)
		{
			continue;
		}
		emptyTestSet = false;

		BigInt::Rossi rossi1 = vtest[i].first;
		BigInt::Rossi rossi2 = vtest[i].second;

		Unit ulong1 = rossi1.toUnit(); 

		rossi1 -= rossi2;
		
		const Unit ulong2 = rossi2.toUnit(); 
		ulong1 -= ulong2;
		
		const BigInt::Rossi rossiResultViaUnit (ulong1);

		const Unit ulongResultViaRossi = rossi1.toUnit();

		ASSERTION(rossi1.getUnitsSize() == rossiResultViaUnit.getUnitsSize());
		if (rossi1.getUnitsSize() == 1)
		{
			ASSERTION(rossiResultViaUnit.getUnitsSize() == 1);
			ASSERTION(rossi1 == rossiResultViaUnit);
			ASSERTION(ulong1 == ulongResultViaRossi);
		}
    }
	ASSERTION(!emptyTestSet);

	SET_FINISH_TEST_NAME(o_stream);

}




// ------------------
void BigInt::PerformanceTestRossi::performanceTestOperatorMultiplication1(std::ostream& o_stream)
{

	SET_START_TEST_NAME(o_stream);

	std::ostringstream ossSource;
	std::ostringstream ossDelta1;
	std::ostringstream ossDelta2;

	ossSource << "abcdef1234";
	ossDelta1 << "1234567890123456789123456789";
	ossDelta2 << std::string (500, 'a');

	const std::size_t preRunsThreshold = 2;

	std::size_t runs = 10;

	std::size_t count = 0;

	std::size_t repetitions = 1;

	for (std::size_t k = 0; k < runs; k++)
	{
		SET3_WORKING_TEST_NAME(std::cerr, k + 1, runs);

		Rossi rossi (ossSource.str().c_str(), HEX_DIGIT);
		Rossi result = Rossi(1);

		START_TIME;
		for (std::size_t i = 0; i < repetitions; i++)
		{
			result = rossi * result;
			s_returnValue+= result.getUnitsSize();
		}

		END_TIME;

		if (s_elapsedTime < s_clockMinThreshold)
		{
			repetitions *= 10;
			runs++;
			continue;
		}

		DISPLAY_TIME(count++, std::cout, rossi, &result,  repetitions, "Rossi::operator*", s_elapsedTime);

		repetitions = 1;

		if (k < (runs - preRunsThreshold))
		{
			ossSource << ossDelta1.str();
		}
		else
		{
			ossSource << ossDelta2.str();
		}
	}

	SET_FINISH_TEST_NAME(o_stream);


}


// ------------------
void BigInt::TestRossi::testOperatorMultiplication1(std::ostream& o_stream)
{
    SET_START_TEST_NAME(o_stream);

    std::vector<std::pair<BigInt::Rossi, BigInt::Rossi> > vtest = BigInt::TestRossi::fillTestInputPairsRossiRossi();

 
    // --- test ---
    for (std::size_t i = 0; i < vtest.size(); i++)
    {
        ROSSI_TEST_COMPUTE_BINARY_OP(o_stream, vtest[i], *);
    }

	SET_FINISH_TEST_NAME(o_stream);

}


// ------------------
void BigInt::TestRossi::testAssertOperatorMultiplication1(std::ostream& o_stream)
{

    SET_START_TEST_NAME(o_stream);

    std::vector<std::pair<BigInt::Rossi, BigInt::Rossi> > vtest = BigInt::TestRossi::fillTestInputPairsRossiRossi();

 
    // --- test ---
	bool emptyTestSet = true;
    for (std::size_t i = 0; i < vtest.size(); i++)
    {
		if (vtest[i].first.getUnitsSize() > 1)
		{
			continue;
		}

		if (vtest[i].second.getUnitsSize() > 1)
		{
			continue;
		}

		BigInt::Rossi rossi1 = vtest[i].first;
		BigInt::Rossi rossi2 = vtest[i].second;
		BigInt::Rossi rossiResult = rossi1 * rossi2;

		if (rossiResult.getUnitsSize() > 1)
		{
			continue;
		}
		emptyTestSet = false;

		const Unit ulong1 = rossi1.toUnit(); 
		const Unit ulong2 = rossi2.toUnit(); 
		const Unit ulongResult = ulong1 * ulong2;

		const BigInt::Rossi rossiResultViaUnit (ulongResult);

		const Unit ulongResultViaRossi = rossiResult.toUnit();

		ASSERTION(rossiResult.getUnitsSize() == rossiResultViaUnit.getUnitsSize());
		if (rossiResult.getUnitsSize() == 1)
		{
			ASSERTION(rossiResultViaUnit.getUnitsSize() == 1);
			ASSERTION(rossiResult == rossiResultViaUnit);
			ASSERTION(ulongResult == ulongResultViaRossi);
		}
    }
	ASSERTION(!emptyTestSet);

	SET_FINISH_TEST_NAME(o_stream);

}



// ------------------
void BigInt::PerformanceTestRossi::performanceTestOperatorMultiplication2(std::ostream& o_stream)
{
	SET_TBD(o_stream);

    SET_START_TEST_NAME(o_stream);

	SET_FINISH_TEST_NAME(o_stream);

}



// ------------------
void BigInt::TestRossi::testOperatorMultiplication2(std::ostream& o_stream)
{
    SET_START_TEST_NAME(o_stream);

    std::vector<std::pair<BigInt::Rossi, BigInt::Unit> > vtest = BigInt::TestRossi::fillTestInputPairsRossiUnit();

 
    // --- test ---
    for (std::size_t i = 0; i < vtest.size(); i++)
    {
        ROSSI_TEST_COMPUTE_BINARY_UNIT_OP(o_stream, vtest[i], *);
    }

	SET_FINISH_TEST_NAME(o_stream);

}


// ------------------
void BigInt::TestRossi::testAssertOperatorMultiplication2(std::ostream& o_stream)
{
    SET_START_TEST_NAME(o_stream);

    std::vector<std::pair<BigInt::Rossi, BigInt::Unit> > vtest = BigInt::TestRossi::fillTestInputPairsRossiUnit();

 
    // --- test ---
	bool emptyTestSet = true;
    for (std::size_t i = 0; i < vtest.size(); i++)
    {
		if (vtest[i].first.getUnitsSize() > 1)
		{
			continue;
		}

		emptyTestSet = false;

		BigInt::Rossi rossi1 = vtest[i].first;
		BigInt::Rossi rossi2 (vtest[i].second);
		BigInt::Rossi rossiResult = rossi1 * rossi2;

		const Unit ulong1 = rossi1.toUnit(); 
		const Unit ulong2 = rossi2.toUnit(); 
		const Unit ulongResult = ulong1 * ulong2;

		const BigInt::Rossi rossiResultViaUnit (ulongResult);

		const Unit ulongResultViaRossi = rossiResult.toUnit();

		ASSERTION(rossiResult.getUnitsSize() == rossiResultViaUnit.getUnitsSize());
		if (rossiResult.getUnitsSize() == 1)
		{
			ASSERTION(rossiResultViaUnit.getUnitsSize() == 1);
			ASSERTION(rossiResult == rossiResultViaUnit);
			ASSERTION(ulongResult == ulongResultViaRossi);
		}
    }

	ASSERTION(!emptyTestSet);

	SET_FINISH_TEST_NAME(o_stream);

}



// ------------------
void BigInt::PerformanceTestRossi::performanceTestOperatorDivision(std::ostream& o_stream)
{
	SET_TBD(o_stream);

    SET_START_TEST_NAME(o_stream);

	SET_FINISH_TEST_NAME(o_stream);

}


// ------------------
void BigInt::TestRossi::testOperatorDivision(std::ostream& o_stream)
{
    SET_START_TEST_NAME(o_stream);

    std::vector<std::pair<BigInt::Rossi, BigInt::Rossi> > vtest = BigInt::TestRossi::fillTestInputPairsRossiRossi();

 
    // --- test ---
    for (std::size_t i = 0; i < vtest.size(); i++)
    {
        if (vtest[i].second == RossiZero)
        {
            continue;
        }
        ROSSI_TEST_COMPUTE_BINARY_OP(o_stream, vtest[i], /);
    }

	SET_FINISH_TEST_NAME(o_stream);

}


// ------------------
void BigInt::TestRossi::testAssertOperatorDivision(std::ostream& o_stream)
{

    SET_START_TEST_NAME(o_stream);

    std::vector<std::pair<BigInt::Rossi, BigInt::Rossi> > vtest = BigInt::TestRossi::fillTestInputPairsRossiRossi();


    // --- test ---
	bool emptyTestSet = true;
    for (std::size_t i = 0; i < vtest.size(); i++)
    {
		if (vtest[i].second == RossiZero)
        {
            continue;
        }

		if (vtest[i].first.getUnitsSize() > 1)
		{
			continue;
		}

		if (vtest[i].second.getUnitsSize() > 1)
		{
			continue;
		}
		emptyTestSet = false;

		BigInt::Rossi rossi1 = vtest[i].first;
		BigInt::Rossi rossi2 = vtest[i].second;
		BigInt::Rossi rossiResult = rossi1 / rossi2;

		const Unit ulong1 = rossi1.toUnit(); 
		const Unit ulong2 = rossi2.toUnit(); 
		const Unit ulongResult = ulong1 / ulong2;

		const BigInt::Rossi rossiResultViaUnit (ulongResult);

		const Unit ulongResultViaRossi = rossiResult.toUnit();

		ASSERTION(rossiResult.getUnitsSize() == rossiResultViaUnit.getUnitsSize());
		if (rossiResult.getUnitsSize() == 1)
		{
			ASSERTION(rossiResultViaUnit.getUnitsSize() == 1);
			ASSERTION(rossiResult == rossiResultViaUnit);
			ASSERTION(ulongResult == ulongResultViaRossi);
		}
    }

	ASSERTION(!emptyTestSet);

	SET_FINISH_TEST_NAME(o_stream);

}



// ------------------
void BigInt::PerformanceTestRossi::performanceTestOperatorReminder(std::ostream& o_stream)
{
	SET_TBD(o_stream);

    SET_START_TEST_NAME(o_stream);

	SET_FINISH_TEST_NAME(o_stream);

}



// ------------------
void BigInt::TestRossi::testOperatorReminder(std::ostream& o_stream)
{
    SET_START_TEST_NAME(o_stream);

    std::vector<std::pair<BigInt::Rossi, BigInt::Rossi> > vtest = BigInt::TestRossi::fillTestInputPairsRossiRossi();

 
    // --- test ---
    for (std::size_t i = 0; i < vtest.size(); i++)
    {
        if (vtest[i].second == RossiZero)
        {
            continue;
        }
        ROSSI_TEST_COMPUTE_BINARY_OP(o_stream, vtest[i], %);
    }

	SET_FINISH_TEST_NAME(o_stream);

}



// ------------------
void BigInt::TestRossi::testAssertOperatorReminder(std::ostream& o_stream)
{

    SET_START_TEST_NAME(o_stream);

    std::vector<std::pair<BigInt::Rossi, BigInt::Rossi> > vtest = BigInt::TestRossi::fillTestInputPairsRossiRossi();

 
    // --- test ---
	bool emptyTestSet = true;
    for (std::size_t i = 0; i < vtest.size(); i++)
    {
		if (vtest[i].second == RossiZero)
        {
            continue;
        }

		if (vtest[i].first.getUnitsSize() > 1)
		{
			continue;
		}

		if (vtest[i].second.getUnitsSize() > 1)
		{
			continue;
		}
		emptyTestSet = false;

		BigInt::Rossi rossi1 = vtest[i].first;
		BigInt::Rossi rossi2 = vtest[i].second;
		BigInt::Rossi rossiResult = rossi1 % rossi2;

		const Unit ulong1 = rossi1.toUnit(); 
		const Unit ulong2 = rossi2.toUnit(); 
		const Unit ulongResult = ulong1 % ulong2;

		const BigInt::Rossi rossiResultViaUnit (ulongResult);

		const Unit ulongResultViaRossi = rossiResult.toUnit();

		ASSERTION(rossiResult.getUnitsSize() == rossiResultViaUnit.getUnitsSize());
		if (rossiResult.getUnitsSize() == 1)
		{
			ASSERTION(rossiResultViaUnit.getUnitsSize() == 1);
			ASSERTION(rossiResult == rossiResultViaUnit);
			ASSERTION(ulongResult == ulongResultViaRossi);
		}
    }

	ASSERTION(!emptyTestSet);

	SET_FINISH_TEST_NAME(o_stream);

}


// ------------------
void BigInt::TestRossi::testOperatorLess(std::ostream& o_stream)
{
    SET_START_TEST_NAME(o_stream);

    std::vector<std::pair<BigInt::Rossi, BigInt::Rossi> > vtest = BigInt::TestRossi::fillTestInputPairsRossiRossi();

 
    // --- test ---
    for (std::size_t i = 0; i < vtest.size(); i++)
    {
        BIGINT_TEST_COMPARE_BINARY_OP(o_stream, vtest[i], <);
    }

	SET_FINISH_TEST_NAME(o_stream);

}



// ------------------
void BigInt::PerformanceTestRossi::performanceTestOperatorLess(std::ostream& o_stream)
{
	SET_TBD(o_stream);

    SET_START_TEST_NAME(o_stream);

	SET_FINISH_TEST_NAME(o_stream);

}




// ------------------
void BigInt::TestRossi::testAssertOperatorLess(std::ostream& o_stream)
{

    SET_START_TEST_NAME(o_stream);

    std::vector<std::pair<BigInt::Rossi, BigInt::Rossi> > vtest = BigInt::TestRossi::fillTestInputPairsRossiRossi();

 
    // --- test ---
	bool emptyTestSet = true;
    for (std::size_t i = 0; i < vtest.size(); i++)
    {
		if (vtest[i].first.getUnitsSize() > 1)
		{
			continue;
		}

		if (vtest[i].second.getUnitsSize() > 1)
		{
			continue;
		}
		emptyTestSet = false;

		BigInt::Rossi rossi1 = vtest[i].first;
		BigInt::Rossi rossi2 = vtest[i].second;
		const bool rossiResult = rossi1 < rossi2;

		const Unit ulong1 = rossi1.toUnit(); 
		const Unit ulong2 = rossi2.toUnit(); 
		const bool ulongResult = ulong1 < ulong2;

		ASSERTION (rossiResult == ulongResult);
    }
	ASSERTION(!emptyTestSet);

	SET_FINISH_TEST_NAME(o_stream);

}



    
// ------------------
void BigInt::TestRossi::testAll(std::ostream& o_stream)
{
	testPrintAll(o_stream);
    testOperatorAdd(o_stream);
    testOperatorAddAssign(o_stream);
    testOperatorSubtraction(o_stream);
    testOperatorSubtractionAssign(o_stream);
    testOperatorMultiplication1(o_stream);
    testOperatorMultiplication2(o_stream);
    testOperatorDivision(o_stream);
    testOperatorReminder(o_stream);
	testOperatorLess(o_stream);
	testSqrt(o_stream);
	testPowAll(o_stream);
	testDoubleAll(o_stream);
}


   
// ------------------
void BigInt::PerformanceTestRossi::performanceTestAll(std::ostream& o_stream)
{

    performanceTestOperatorAdd(o_stream);
    performanceTestOperatorAddAssign(o_stream);
    performanceTestOperatorSubtraction(o_stream);
    performanceTestOperatorSubtractionAssign(o_stream);
    performanceTestOperatorMultiplication1(o_stream);
    performanceTestOperatorMultiplication2(o_stream);
    performanceTestOperatorDivision(o_stream);
    performanceTestOperatorReminder(o_stream);
	performanceTestOperatorLess(o_stream);
	performanceTestSqrt(o_stream);
	performanceTestPow(o_stream);
	performanceTestDouble(o_stream);
}


    
// ------------------
void BigInt::TestRossi::testAssertAll(std::ostream& o_stream)
{
	testAssertPrintAll(o_stream);
    testAssertOperatorAdd(o_stream);
    testAssertOperatorAddAssign(o_stream);
    testAssertOperatorSubtraction(o_stream);
    testAssertOperatorSubtractionAssign(o_stream);
    testAssertOperatorMultiplication1(o_stream);
    testAssertOperatorMultiplication2(o_stream);
    testAssertOperatorDivision(o_stream);
    testAssertOperatorReminder(o_stream);
	testAssertOperatorLess(o_stream);
	testAssertSqrt(o_stream);
	testAssertPowAll(o_stream);
	testAssertDoubleAll(o_stream);

}



// ------------------
void BigInt::PerformanceTestVin::performanceTestOperatorAdd(std::ostream& o_stream)
{

    SET_START_TEST_NAME(o_stream);

	std::ostringstream ossSource;
	std::ostringstream ossDelta1;
	std::ostringstream ossDelta2;

	ossSource << "abcdef1234";
	ossDelta1 << "1234567890123456789123456789";
	ossDelta2 << std::string (2500, 'a');

	const std::size_t preRunsThreshold = 2;


	std::size_t runs = 10;

	std::size_t repetitions = 1;

	std::size_t count = 0;

	for (std::size_t k = 0; k < runs; k++)
	{
		SET3_WORKING_TEST_NAME(std::cerr, k + 1, runs);

		Vin vin (ossSource.str().c_str(), HEX_DIGIT);
		Vin result (0);

		START_TIME;
		for (std::size_t i = 0; i < repetitions; i++)
		{
			result = vin + result;
			s_returnValue+= result.getUnitsSize();
		}
		END_TIME;

		if (s_elapsedTime < s_clockMinThreshold)
		{
			repetitions *= 10;
			runs++;
			continue;
		}


		DISPLAY_TIME(count++, std::cout, vin, &result,  repetitions, "Vin::operator+", s_elapsedTime);

		repetitions = 1;

		if (k < (runs - preRunsThreshold))
		{
			ossSource << ossDelta1.str();
		}
		else
		{
			ossSource << ossDelta2.str();
		}

		

	}

	SET_FINISH_TEST_NAME(o_stream);

}



// ------------------
void BigInt::TestVin::testOperatorAdd(std::ostream& o_stream)
{
    SET_START_TEST_NAME(o_stream);

    std::vector<std::pair<BigInt::Vin, BigInt::Vin> > vtest = BigInt::TestVin::fillTestInputPairsVinVin();

 
    // --- test ---
    for (std::size_t i = 0; i < vtest.size(); i++)
    {
        VIN_TEST_COMPUTE_BINARY_OP(o_stream, vtest[i], +);
    }

	SET_FINISH_TEST_NAME(o_stream);

}


// ------------------
void BigInt::TestVin::testAssertOperatorAdd(std::ostream& o_stream)
{
    SET_START_TEST_NAME(o_stream);

    std::vector<std::pair<BigInt::Vin, BigInt::Vin> > vtest = BigInt::TestVin::fillTestInputPairsVinVin();

 
    // --- test ---
	bool emptyTestSet = true;
    for (std::size_t i = 0; i < vtest.size(); i++)
    {
		if (vtest[i].first.getUnitsSize() > 1)
		{
			continue;
		}

		if (vtest[i].second.getUnitsSize() > 1)
		{
			continue;
		}

		BigInt::Vin vin1 = vtest[i].first;
		BigInt::Vin vin2 = vtest[i].second;
		BigInt::Vin vinResult = vin1 + vin2;

		const Unit ulong1 = vin1.toUnit(); 
		const Unit ulong2 = vin2.toUnit(); 
		const Unit ulongResult = ulong1 + ulong2;

		if (ulongResult >= VIN_BASE2)
		{
			continue;
		}
		emptyTestSet = false;

		const BigInt::Vin vinResultViaUnit (ulongResult);

		const Unit ulongResultViaVin = vinResult.toUnit();

		ASSERTION(vinResult.getUnitsSize() == vinResultViaUnit.getUnitsSize());
		if (vinResult.getUnitsSize() == 1)
		{
			ASSERTION(vinResultViaUnit.getUnitsSize() == 1);
			ASSERTION(vinResult == vinResultViaUnit);
			ASSERTION(ulongResult == ulongResultViaVin);
		}
    }
	ASSERTION(!emptyTestSet);

	SET_FINISH_TEST_NAME(o_stream);

}





// ------------------
void BigInt::TestVin::testOperatorMultiplication(std::ostream& o_stream)
{
    SET_START_TEST_NAME(o_stream);

    std::vector<std::pair<BigInt::Vin, BigInt::Unit> > vtest = BigInt::TestVin::fillTestInputPairsVinUnit();

 
    // --- test ---
    for (std::size_t i = 0; i < vtest.size(); i++)
    {
        VIN_TEST_COMPUTE_BINARY_UNIT_OP(o_stream, vtest[i], *);
    }

	SET_FINISH_TEST_NAME(o_stream);

}


// ------------------
void BigInt::PerformanceTestVin::performanceTestOperatorLess(std::ostream& o_stream)
{
	SET_TBD(o_stream);

    SET_START_TEST_NAME(o_stream);

	SET_FINISH_TEST_NAME(o_stream);

}



// ------------------
void BigInt::TestVin::testOperatorLess(std::ostream& o_stream)
{
    SET_START_TEST_NAME(o_stream);

    std::vector<std::pair<BigInt::Vin, BigInt::Vin> > vtest = BigInt::TestVin::fillTestInputPairsVinVin();

 
    // --- test ---
    for (std::size_t i = 0; i < vtest.size(); i++)
    {
        BIGINT_TEST_COMPARE_BINARY_OP(o_stream, vtest[i], <);
    }

	SET_FINISH_TEST_NAME(o_stream);

}


// ------------------
void BigInt::TestVin::testAssertOperatorLess(std::ostream& o_stream)
{

    SET_START_TEST_NAME(o_stream);

    std::vector<std::pair<BigInt::Vin, BigInt::Vin> > vtest = BigInt::TestVin::fillTestInputPairsVinVin();

 
    // --- test ---
	bool emptyTestSet = true;
    for (std::size_t i = 0; i < vtest.size(); i++)
    {
		if (vtest[i].first.getUnitsSize() > 1)
		{
			continue;
		}

		if (vtest[i].second.getUnitsSize() > 1)
		{
			continue;
		}
		emptyTestSet = false;

		BigInt::Vin vin1 = vtest[i].first;
		BigInt::Vin vin2 = vtest[i].second;
		const bool vinResult = vin1 < vin2;

		const Unit ulong1 = vin1.toUnit(); 
		const Unit ulong2 = vin2.toUnit(); 
		const bool ulongResult = ulong1 < ulong2;

		ASSERTION (vinResult == ulongResult);
    }
	ASSERTION(!emptyTestSet);


	SET_FINISH_TEST_NAME(o_stream);

}


    
// ------------------
void BigInt::TestVin::testAll(std::ostream& o_stream)
{
    testOperatorAdd(o_stream);
    // testOperatorMultiplication();
    testOperatorLess(o_stream);
}

   
// ------------------
void BigInt::PerformanceTestVin::performanceTestAll(std::ostream& o_stream)
{
    performanceTestOperatorAdd(o_stream);
    // testOperatorMultiplication();
    performanceTestOperatorLess(o_stream);
}

   
// ------------------
void BigInt::TestVin::testAssertAll(std::ostream& o_stream)
{
    testAssertOperatorAdd(o_stream);
    testAssertOperatorLess(o_stream);
}




// --------------------------
bool BigInt::assertCheck(const bool i_preCheck)
{

#define CHECK_STATMENT(b, t) \
	if (b) \
	{ \
		if (!(t)) \
	    { \
			return false; \
	    } \
    } \
	else \
	{ \
		ASSERTION (t); \
	} \


	// ------------------------------------
	CHECK_STATMENT (i_preCheck, (BigInt::VIN_SUB_BASE2 == (VIN_BASE2 - 1)));
    CHECK_STATMENT (i_preCheck, (!(BigInt::VIN_BASE2 >= BigInt::VIN_UNIT_MAX)));
    CHECK_STATMENT (i_preCheck,  (BigInt::VIN_BASE1 * (BigInt::VIN_BASE2/VIN_BASE1 + 1) < BigInt::VIN_UNIT_MAX));
    CHECK_STATMENT (i_preCheck,  (!(BigInt::VIN_BASE2 != (BigInt::VIN_SUB_BASE2 + 1))));
    CHECK_STATMENT (i_preCheck,  (BigInt::VIN_OVER_BASE2 > BigInt::VIN_SUB_BASE2));

    CHECK_STATMENT (i_preCheck, (
            ((sizeof(BigInt::Unit) == 4) && ((BigInt::UNIT_MSB == static_cast<BigInt::Unit>(0x80000000))))
            ||
            ((sizeof(BigInt::Unit) == 8) && ((BigInt::UNIT_MSB == ((static_cast<BigInt::Unit>(0x80000000) << 31) << 1))))
		  ));

	CHECK_STATMENT (i_preCheck,  (VIN_UNIT_MAX == (std::numeric_limits<Unit>::max() >> 2)));
	CHECK_STATMENT (i_preCheck,  (UNIT_MAX == std::numeric_limits<Unit>::max()));


	// -----------------------------------
	CHECK_STATMENT (i_preCheck,  (BigInt::VIN_BASE1 == BigInt::get_VIN_BASE1(BigInt::VIN_UNIT_MAX)));
	CHECK_STATMENT (i_preCheck,  (BigInt::VIN_BASE2 == BigInt::get_VIN_BASE2(BigInt::get_VIN_BASE1(BigInt::VIN_UNIT_MAX))));

	// -----------------------------------
    std::ostringstream oss_VIN_UNIT_MAX;
	std::ostringstream oss_VIN_BASE2;

	oss_VIN_UNIT_MAX << BigInt::VIN_UNIT_MAX;
	oss_VIN_BASE2    << BigInt::VIN_BASE2;

	const std::size_t decDigitsIn_VIN_UNIT_MAX = oss_VIN_UNIT_MAX.str().size();
	const std::size_t decDigitsIn_VIN_BASE2    = oss_VIN_BASE2.str().size();
	
	CHECK_STATMENT (i_preCheck,  (decDigitsIn_VIN_UNIT_MAX == VIN_BASE1));
	CHECK_STATMENT (i_preCheck,  (decDigitsIn_VIN_BASE2 == VIN_BASE1));
	CHECK_STATMENT (i_preCheck,  (decDigitsIn_VIN_UNIT_MAX == decDigitsIn_VIN_BASE2));
	
	BigInt::Unit base2 = 1;
	for (std::size_t i = 0; i < static_cast<std::size_t>(BigInt::VIN_BASE1 - 1); i++)
	{
		base2 *= 10;
	}

	CHECK_STATMENT (i_preCheck,  (BigInt::VIN_BASE2 == base2));

	// ---------------------------------------------
	// This assert is to avoid possible problematic situation for Windows 64-bits where
	//  sizeof (std::size_t) == 8, but sizeof (unsigned long) == 4
	//  If ASSERTION() failed then "typedef Unit" should be changed
	// CHECK_STATMENT (i_preCheck,  (sizeof(std::size_t) <= sizeof(Unit)));
	// ---------------------------------------------

	return true;

#undef CHECK_STATMENT

}  
	


// ------------------
void BigInt::Test::auxDisplayTime(
						const std::size_t	i_serialNo, 
						std::ostream&		o_stream, 
						const std::string&	i_operationName,
						const std::size_t	i_totalOperations, 
						const BaseBigInt&	i_ins1, 
						const BaseBigInt*	i_ptr2,
						const clock_t		i_elapsedTime
						)
{
	std::ostringstream oss;
	std::ostringstream ossMsg;

	ossMsg << ""
		   << "Performance test "
		   << std::setw (2)
		   << i_serialNo
		   << " ---> ";
	const std::string strMsg = ossMsg.str();
		
		              
	oss.precision (std::numeric_limits<double>::digits10);
	oss	<< "" 
		<< std::endl 
		<< strMsg 
		<< "BigInt's size "
		<< ((i_ptr2 == 0) ? "" : "range ")
		<< "is:  "
		<< i_ins1.getUnitsSize(); 

	if (i_ptr2 != 0) 
	{ 
		oss << " - "
			<< i_ptr2->getUnitsSize(); 
	}
	
	oss	<< " Unit's"
		<< " ("
		<< i_ins1.getActualHexDigits(); 

	if (i_ptr2 != 0)
	{
		oss	<< " - "
		<< i_ptr2->getActualHexDigits(); 
	}

	oss	<< " hexadecimal digits"
		<< ") "
		<< std::endl
		<< std::string(strMsg.size(), ' ')
		<< i_totalOperations
		<< " operation"
		<< ((i_totalOperations > 1) ? "s" : "")
		<< " "
		<< "\""
		<< i_operationName
		<< "\""
		<< " took: "
		<< std::fixed
		<< i_elapsedTime
		<< " clicks ("
		<< std::fixed
		<< static_cast<double>(i_elapsedTime)/CLOCKS_PER_SEC
		<< " seconds)"
		<< std::endl
		<< std::string(strMsg.size(), ' ')
		<< "Single operation "
		<< "\""
		<< i_operationName
		<< "\""
		<< " took: "
		<< std::fixed
		<< (static_cast<double>(i_elapsedTime)/i_totalOperations)
		<< " clicks ("
		<< std::fixed
		<< (static_cast<double>(i_elapsedTime)/CLOCKS_PER_SEC)/i_totalOperations
		<< " seconds)"
		<< std::endl;

	o_stream << oss.str() << std::flush; \
									
		
}



// ------------------
void BigInt::Test::setTestName(std::ostream& o_stream, const std::string& i_txt, const std::string& i_funcName, const std::size_t i_lineNo, const std::size_t i_counter, const std::size_t i_total)
{
    std::ostringstream oss;
	std::ostringstream ossNameLineNo;

	ossNameLineNo << "" 
		          << i_txt;

	if (i_counter != std::numeric_limits<std::size_t>::max())
	{
		ossNameLineNo << ""
			          << "-"
					  << i_counter;

		if (i_total != std::numeric_limits<std::size_t>::max())
		{
			ossNameLineNo << ""
				          << "-of-"
						  << i_total;
		}
	}

	ossNameLineNo << ": "
		          << i_funcName
		          << "; "
				  << "lineNo = "
		          << i_lineNo;

	const std::size_t nameLineNoSize = ossNameLineNo.str().size();
   
    oss << std::endl
        << std::endl
        << std::endl

        << std::string (nameLineNoSize, '-') 
        << std::endl

        << ossNameLineNo.str()
        << std::endl

        << std::string (nameLineNoSize, '-')
        << std::endl

        << std::endl;

    o_stream << std::flush << oss.str() << std::flush;
}


// ------------------
void BigInt::Test::testDisplayBigInts(std::ostream& o_stream)
{
    std::ostringstream oss;

    SET_START_TEST_NAME(oss);

    const std::string arrayHexStrNumbers[] = 
    {
        BigInt::toString(0),
        BigInt::toString(1),
        BigInt::toString(2),
        BigInt::toString(9),
        BigInt::toString(10),
        BigInt::toString(11),
        BigInt::toString(15),
        BigInt::toString(16),
        BigInt::toString(17),
        BigInt::toString(std::numeric_limits<BigInt::Unit>::max() - 1),
        BigInt::toString(std::numeric_limits<BigInt::Unit>::max())
       
    };

    const std::vector<std::string> vectorHexStrNumbers (array2vector (arrayHexStrNumbers));

    // ----------------------------------
    std::set<BigInt::Vin> vinBigInts;

    for (std::size_t i = 0; i < vectorHexStrNumbers.size(); i++)
    {
        for (std::size_t k = 0; k < vectorHexStrNumbers.size(); k++)
        {
            BigInt::Vin value1 (vectorHexStrNumbers[i], DEC_DIGIT);
            BigInt::Vin value2 (vectorHexStrNumbers[k], DEC_DIGIT);
            BigInt::Vin sum = value1 + value2; 
           
            vinBigInts.insert(sum);
        }
    }


    // -----------------------------------------------
    std::set<BigInt::Vin>::const_iterator iter;
    for (iter = vinBigInts.begin();
         iter != vinBigInts.end();
         iter++
         )
    {
        const BigInt::Vin&  curVin = *iter;
        const BigInt::Rossi curRossi (curVin);

        ASSERTION (curVin.toStrDec() == curRossi.toStrDec());

        oss << ""
            << curRossi.toStrDec()
            << " = "
            << curRossi.toStr0xHex()
            << std::endl;
    }

	SET_FINISH_TEST_NAME(oss);
    // ------------------------------
    o_stream << std::flush << oss.str() << std::flush;
}


// ------------------
void BigInt::Test::testMain(std::ostream& o_stream, const std::vector<std::string>& i_args)
{
    std::ostringstream oss;

    SET_START_TEST_NAME(oss);

    BigInt::Run::mainBigInt(oss, i_args);

	SET_FINISH_TEST_NAME(oss);

    // ------------------------------
    o_stream << std::flush << oss.str() << std::flush;

}




// ------------------
std::vector<std::string> BigInt::Test::fillTestInputHexStr()
{
    // --- hexStrNumbers ---

    const std::string arrayHexStrNumbers[] = 
    {
        "0",
        "0",
        "1",
        "1",
        "2",
        "3",
        "100",
        "ABC",
        "100000000"
        "123456789ABCDEF",
        "F000000000000000",
        "FEDCBA9876543210",
        "10000000000000000",
        "1000000000000000000000000",
        "111222333444555666777888999AAABBBCC"
    };

    const std::vector<std::string> retVector (array2vector (arrayHexStrNumbers));

    return retVector;

}


// ------------------
std::vector<BigInt::Unit> BigInt::TestRossi::fillTestInputUnit()
{
    // --- ulongNumbers ---
    const BigInt::Unit arrayUnitNumbers[] = 
    {
        0, 
        0, 
        1, 
        1, 
        2, 
        3, 
        9, 
        10, 
        11, 
        15, 
        16, 
        17, 
    };
    const std::vector<BigInt::Unit> retVector (array2vector (arrayUnitNumbers));
   
    return retVector;

}


// ------------------
std::vector<BigInt::Unit> BigInt::TestVin::fillTestInputUnit()
{
    // --- ulongNumbers ---
    const BigInt::Unit arrayUnitNumbers[] = 
    {
        0, 
        0, 
        1, 
        1, 
        2, 
        3, 
        9, 
        10, 
        11, 
        15, 
        16, 
        17, 
        999999998, 
        999999999
    };
    const std::vector<BigInt::Unit> retVector (array2vector (arrayUnitNumbers));
   
    return retVector;

}





// ------------------
std::vector<std::pair<BigInt::Rossi, BigInt::Rossi> > BigInt::TestRossi::fillTestInputPairsRossiRossi()
{
    std::vector<std::pair<BigInt::Rossi, BigInt::Rossi> > retVector;

    
    std::vector<BigInt::Unit> ulongNumbers(BigInt::TestRossi::fillTestInputUnit());
    std::vector<std::string>   hexStrNumbers(BigInt::Test::fillTestInputHexStr());
    // ------------------------------------

    for (std::size_t i = 0; i < ulongNumbers.size(); i++)
    {
        for (std::size_t k = 0; k < ulongNumbers.size(); k++)
        {
            retVector.push_back(
                std::make_pair(
                        BigInt::Rossi(ulongNumbers[i]), 
                        BigInt::Rossi(ulongNumbers[k])
                        ));
        }
    }

    for (std::size_t i = 0; i < hexStrNumbers.size(); i++)
    {
        for (std::size_t k = 0; k < hexStrNumbers.size(); k++)
        {
            retVector.push_back(
                std::make_pair(
                        BigInt::Rossi(hexStrNumbers[i], BigInt::HEX_DIGIT), 
                        BigInt::Rossi(hexStrNumbers[k], BigInt::HEX_DIGIT)
                        ));
        }
    }


    return retVector;


}

// ------------------
std::vector<std::pair<BigInt::Rossi, BigInt::Unit> > BigInt::TestRossi::fillTestInputPairsRossiUnit()
{
    std::vector<std::pair<BigInt::Rossi, BigInt::Unit> > retVector;
    
    std::vector<BigInt::Unit> ulongNumbers(BigInt::TestRossi::fillTestInputUnit());
    // ------------------------------------

    for (std::size_t i = 0; i < ulongNumbers.size(); i++)
    {
        for (std::size_t k = 0; k < ulongNumbers.size(); k++)
        {
            retVector.push_back(
                std::make_pair(
                        BigInt::Rossi(ulongNumbers[i]), 
                        ulongNumbers[k]
                        ));
        }
    }

    return retVector;

}


// ------------------
std::vector<std::pair<BigInt::Vin, BigInt::Vin> > BigInt::TestVin::fillTestInputPairsVinVin()
{
    std::vector<std::pair<BigInt::Vin, BigInt::Vin> > retVector;

    
    std::vector<BigInt::Unit> ulongNumbers (BigInt::TestVin::fillTestInputUnit());
    std::vector<std::string>   hexStrNumbers (BigInt::Test::fillTestInputHexStr());
    // ------------------------------------

    for (std::size_t i = 0; i < ulongNumbers.size(); i++)
    {
        for (std::size_t k = 0; k < ulongNumbers.size(); k++)
        {
            retVector.push_back(
                std::make_pair(
                        BigInt::Vin(ulongNumbers[i]), 
                        BigInt::Vin(ulongNumbers[k])
                        ));
        }
    }

    for (std::size_t i = 0; i < hexStrNumbers.size(); i++)
    {
        for (std::size_t k = 0; k < hexStrNumbers.size(); k++)
        {
            retVector.push_back(
                std::make_pair(
                        BigInt::Vin(hexStrNumbers[i], BigInt::HEX_DIGIT), 
                        BigInt::Vin(hexStrNumbers[k], BigInt::HEX_DIGIT)
                        ));
        }
    }


    return retVector;


}

// ------------------
std::vector<std::pair<BigInt::Vin, BigInt::Unit> > BigInt::TestVin::fillTestInputPairsVinUnit()
{
    std::vector<std::pair<BigInt::Vin, BigInt::Unit> > retVector;

    
    std::vector<BigInt::Unit> ulongNumbers (BigInt::TestVin::fillTestInputUnit());
    // ------------------------------------

    for (std::size_t i = 0; i < ulongNumbers.size(); i++)
    {
        for (std::size_t k = 0; k < ulongNumbers.size(); k++)
        {
            retVector.push_back(
                std::make_pair(
                        BigInt::Vin(ulongNumbers[i]), 
                        ulongNumbers[k]
                        ));
        }
    }

    return retVector;

}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//ʹ�������


#if 0
// ==============================================================
//
//  Copyright (C) 1995  William A. Rossi
//                      class RossiBigInt
// 
//  Copyright (C) 1999-2016  Alex Vinokur
//                           class BigInt 
//                           class BigInt::BaseBigInt 
//                           class BigInt::Vin
//                           upgrading class BigInt::Rossi
//                           class BigInt::Run
//                           class BigInt::Test
//                           class BigInt::TestVin
//                           class BigInt::TestRossi
//			     class BigInt::PerformanceTestVin
//                           class BigInt::PerformanceTestRossi
//
//  ------------------------------------------------------------
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//  ------------------------------------------------------------
// 
//  mailto:alex DOT vinokur AT gmail DOT com
//  http://sourceforge.net/users/alexvn/
//
// ==============================================================


C++ BigInt Class
    ---------------
    Latest Version: 10.2.

    C++ BigInt class enables the user to work with arbitrary precision integers.

    BigInt is represented by STL vector of Unit's, where Unit is unsigned integer of 64-bits (default) or 32-bits - for both 64-bits and 32-bits Operating Systems.

    C++ BigInt class contains also computing and performance tests.


    =================================== Software Files ==================================

    -------------------------------------------------------------------------------------
    * bigInt.h
    * bigInt.cpp
    * main.cpp

    -------------------------------------------------------------------------------------


    ==================================== Compilation ====================================

    Compilation samples (for g++)

    -------------------------------------------------------------------------------------
    // Unit - 64 bits   (Default)
    // Use of try-catch (Default)
    g++ bigInt.cpp main.cpp

    ------------------------------
    // Unit - 32 bits
    // Use of try-catch (Default)
    g++ -DUNIT32 bigInt.cpp main.cpp

    ------------------------------
    // Unit - 64 bits            (Default)
    // Limited use of try-catch 
    g++ -DNO_TRY_CATCH bigInt.cpp main.cpp

    ------------------------------
    // Unit - 32 bits
    // Limited use of try-catch 
    g++ -DUNIT32 -DNO_TRY_CATCH bigInt.cpp main.cpp

    -------------------------------------------------------------------------------------



    ======================================= Usage =======================================

    -------------------------------------------------------------------------------------
USAGE : 
: <exeFileName>        // help
    : <exeFileName> help   // help
    : <exeFileName> demo   // demo
    : <exeFileName> test   // computing tests
    : <exeFileName> time   // performance tests
    : <exeFileName> <first-arg-in-decimal> <binary-operation> <second-arg-in-decimal>  // bigint's calculator


    USAGE SAMPLE: 
: ./bigint.exe
    : ./bigint.exe help
    : ./bigint.exe demo
    : ./bigint.exe test
    : ./bigint.exe time
    : ./bigint.exe 1123581321345589 + 123456789


    -------------------------------------------------------------------------------------



    ==================================== Simple Demo ====================================

    -------------------------------------- Program --------------------------------------

#include "bigInt.h"
    void simpleDemoBigInt ()
{

#define SHOW_BIG_INT(x) std::cout << #x << " : Hex Value = " << x.toStr0xHex() << ", Hex Digits = " << x.getActualHexDigits() << "; Dec Value = " << x.toStrDec() << ", Dec Digits = " << x.getActualDecDigits() << std::endl << std::endl
#define SHOW_DOUBLE(x) std::cout << #x << " : " << std::fixed << x << std::endl << std::endl

    BigInt::Rossi n1 ("314159265358979323846264338327950288419716939937510", BigInt::DEC_DIGIT);
    SHOW_BIG_INT(n1);

    BigInt::Rossi n2 ("224f3e07282886cce82404b6f8", BigInt::HEX_DIGIT);
    SHOW_BIG_INT(n2);

    BigInt::Rossi n3 (1123);
    SHOW_BIG_INT(n3);

    BigInt::Rossi n4 = n1 + n2;
    SHOW_BIG_INT(n4);

    BigInt::Rossi n5 = n1 * n3;
    SHOW_BIG_INT(n5);

    BigInt::Rossi n6 = n2 / n3;
    SHOW_BIG_INT(n6);

    BigInt::Rossi n7 = n1.sqrt();
    SHOW_BIG_INT(n7);

    BigInt::Rossi n8 = BigInt::Rossi::pow (n1, n3);
    SHOW_BIG_INT(n8);

    long double d1 = n1.toDouble();
    SHOW_DOUBLE(d1);

    long double d2 = 1123581321345589144233.0;
    SHOW_DOUBLE(d2);

    BigInt::Rossi n9 = BigInt::Rossi::fromDouble(d2);
    SHOW_BIG_INT(n9);

#undef SHOW_BIG_INT
#undef SHOW_DOUBLE


}



--------------------------------------- Output --------------------------------------

n1 : Hex Value = 0xd6f4dcacdbe68004d7666a4d01dfa3321e33e476e6, Hex Digits = 42; Dec Value = 314159265358979323846264338327950288419716939937510, Dec Digits = 51

n2 : Hex Value = 0x224f3e07282886cce82404b6f8, Hex Digits = 26; Dec Value = 2718281828459045235360287471352, Dec Digits = 31

n3 : Hex Value = 0x463, Hex Digits = 3; Dec Value = 1123, Dec Digits = 4

n4 : Hex Value = 0xd6f4dcacdbe68004f9b5a8542a0829ff0657e92dde, Hex Digits = 42; Dec Value = 314159265358979323848982620156409333655077227408862, Dec Digits = 51

n5 : Hex Value = 0x3aef4240a48a423953ce6444fcf3808e4da7da33592f2, Hex Digits = 45; Dec Value = 352800854998133780679354851942288173895342123549823730, Dec Digits = 54

n6 : Hex Value = 0x7d23c3022afa65dabafc311, Hex Digits = 23; Dec Value = 2420553720800574563989570321, Dec Digits = 28

n7 : Hex Value = 0xea951286b8be7b4ce8eaa, Hex Digits = 21; Dec Value = 17724538509055160272981674, Dec Digits = 26

n8 : Hex Value = 0x1db4bae518dae7b21b2326b187f5128ff8bff6f94518935f0613d236f24ed07ebce0d240033a7b4e9e31223f219676a2c8980a17cbe7406adc05177a96df72ca80c28ec2d9e486f4df2b58fb86f8829477edb456eeae811bcd27ffaa30174f99d4e3cfe1a476394ea3062002ca8f6ec6e84888bdaf8fccec1ea8bf15a1f5b2e2b682304b94876c9cd5c53f30b11f4dcd8ab7bd2dde2636964b19cfd9a9bbbf9857f48b2a1c4c11015d9abd9649eca7a18c0f1d4ca0926836edd0ce6186bd70eca76e58a6ead2c1cbd1380fab5e64a36e57244d78f8e3f5e9a943b01766e108f1add29d4520b72084710a30aef73685c7b848c1e282c392e143d83eebf87552e14b3f9d56ae5411e735ef904c53f53dcaa43dcf0fc07eb439a0167392206d92e633ef2a6acca689d7102ddb0a860d0a5f6dcb5730effc593e6011369a906ae95940859be4736ad54f7c3d6a5587413290bb0c8ed2691b37dbd9d820a96685984cdd886c5fa454fd9222a40ad2c7f7eafc21cbc4311d4c51c3f19fa1862e091aca3ebb8341a10f50de1167d77dbe1200bac1d9b1098bf2cdb95a4cb3a80c3cdbbe0ffe2718835f5c32e6037da0ccf01a2724a9b63cc57873d16a7a01cdbcbf9964d227761253c922c2311a383e36720a5f7e6969445dc023a6fde525622935824519f427066ae1be2b4dabc07bd2520df4997378f76a886b25ea393edb8e9b38f295ceaa0205517c76d644d658f8bde7919d09e4b5975a27a2789108905a9914a844e2473e593d64d167b9850380c3a65ffadef8c37238ade97aa24e855876f572951877abdff9956afdf0310f222b2f91cf634e7dfdca125a42a1f53848e83a88f7e17d3160d60ffc92e804d64135e67f1c5256615d8bed3203992f18f739381fd9b72f28c70df43524a6cda787b8994641f850407b51f4a96bf5595f8611bde8eacd092431da7ce2da5970e1e1d4bc0adc4f0a03917baf014b1f9362eb822fa37f34284b1269813015d9eecb290606c67d86b4f813d35799f78f36aa7bd8efbfdee0569748baf5e9a555bb35f85ed4ef827245da5ee320c6b779f44c8c27be3283bfb43bd98fc174ae0845c99b04357fc504efba2f1de21c890d5e75e0835fbf4722339ddc46c135a67f2f29a822c58fa4be3b5fcc2041a2455e06c5d066ac02a424040279d251d3c971b261469fc87330cf1a92398701f66b96321e549870b6d163facd8e6b8799129b9e800c4683b19fec9d2b820be0b0c12b5604cf0ff633125ba8b55f7e8b46bbced2bb8725edc3b6670987bafb6d0107c517b0a094f984e2e620c7e509cd4372b00f164658ffe158b3ee0930f3b193d9ea65932ebe42d107af17aae7648ce574038ec7f45498c6bdf7ddb18f1b1b41f9b30b2c420f25f2a8159fe3d93fcc1c0ad7c360ac7c38815af881c8877f32acf3450cfdce5f2009cc51b12ff43fedf588b35c966662a7ac937809219a44f764d91fe1cd4b9158c8cb190757f6a52544aea2b256bfbf68954fd47dc0b26bec71988c8ae6b2b498325e48eea4dc7a05b790d49666a93e6c8034a2ce7e69e110d09c910cf7d978bc004a1a098122bd965d5fd93440d7d46d752b52f41f3b720e82f05d939e3a5b316f230ff898751a5e2bd853d5613e77f655315df087d858509015f7ec56fc3b3480240a6314f555d1bae0cc8cef16efa8fc07ed59026557e2b36ca9b7ef2908093fda4e8ae7f6a6782a6634458db27dcc9505eb6e662d9b53e025d3d59757b008655b26e7a84da0cd372fc539c0b7a3cd557124502e2dd8283d9051f89a4a85dbe2ef95ee73d19d0090ae064ff8f2e88f105195110b4ed358e526240b6a88f23f9ae49ac22637e287091615eeb8e39e7b6055c2a91812a38fc0a6872fab858c9902f36171e395defb58ef46639b060ec810a52d97daa7384e77d6c4cee181c482466a5910d19cabc802ca44186a4cceb84f6b2cd4034e661e30d3e4068475131d0d27abede841c1c05cc353a9a255a0a313dfaa6c5ae19960b53068ac5078a570d9fdc71a7f6b47e09902001ba18aecb9563209d8c4da42798139d12985c92023132fc73552ab45f4736529a4943a3a14400c00c92de0282c993ae9b7ae5d66aaa7cf552312a4984a3ce0ba31eb8acf1d28a3d30233149d93ba488ae48fc66a7394d0fbd7ab4a75db5bcd56ae6f9ee0370961a91c4c6468197ccc5c9d55f0f7ca7e02fef1c9775283ef0cb4d633d24316ff379a2b33b82f109f80ea0c616ea04ab2274b4130f8e3db7f2167989020b1fa7618f61fbc7210e33c69ad9bf7dc484329a254cd64222a51153e897dddcc3add4c0aebc9b5bdcba34f189f22de8cbe2ad2ebc609423f353f6b8afee243480bc18ff8e98f784b47fa9c9cb6dbac82751e7c8de2cea16f353d3795b01598e8cc0223e42ecf3bc773733fef4abeac4b0e5dea6580e3eda62d3254a382d9aa0ed8e6653d1a6061ffeab1d4b426b59cedc3206236221229844e4e86a42011f036fd467d6da8334d1ffdecd20aaa0fef6c7f3a8b5d85cd7cd304158197d769ee14cceda617d724778d8a9fa89e3468d6480ee019d466dac13b79dd875e669f44abdabc11a306d38197086f19e75cb74a8ed93db70c300a37023e1098ca80b3fc92a6cb19060523ef263e04aeb4cf0990d30b8fdc0d1b6eadc2713a46058de3644da2399bc76994647213b5018a911eb570d7bf3856fedc25a366fc57efea6e4c354fa52de724e3642b8f250732adea740d4fcb30e7f75187d3e40c964d6a1106199275b84e1e0bc232078adc558cfeec4e72bed87411e50a4996ca383715fed6ee771fd2f9017dd7fbecda7096d67af2ec2f9328358e277cbf31ef2afe036ce3c05732645c1e5bfe9d0d9b48bb4db8319d4cccc7ccb198becabf7b64c40df17a7e6ed26d4b9c2e6ca4eb002fa36de827e85dc1e919d2cc4628ec772e17afbc5d87b4f17cc7bc02dd42fae6cd3f2f4538fec4dd13cdb6d09c74fe00b8162dfda6fbce87e7fcb60df7de64fa281f5349936e3862a7ba6c7e93dc12f4dd7c962b85346220d7edbaece5cbb4dab9848ee881a058af4f99ff2ae6119d179f6a2d85e47e91e4fe8c8eec8d62d65eb9805c3897fd47a296f21db0119e7761fadc1db8ea2e76e009e6503bcff1c3506bdd2fd7b95e14d9756f61db651b762c66063cf82db23a0832363c7b78219fd3c4c1ce2621fd0bd17b99df2f9568bb8053c459de04e09c39a7818a70d3edd90cb24f27a328785b41cc9e90fbd91dd1f65fd31c8216f317c726d5ba045e3e888716606180b3d6843dc6e0efa024207dff3cbc54d8d75a3a2b1996a86cbb537c467f679744631652e83aa26f1ce50e6a69b645a0837950c6ad21b245423fdb60b2a805a816de36fc2df7b05223c34e06dde69d0fe43ff91c0c1a149c250f5bd26be3fcb7cbb3b6451858a0f8c08949a24aa94e08b36fcb995fe569532331b87c36aaa111550d89f293bc9c02ea04436d2fe1a7fd936efdd64a8f5e6ef755f3066942a67196684ba70918385f0fdf2bafa79ad66241ce2dc3ce980616932ca2624573f8a56380abbb9b8e4c6753ee1aedc15790ec33c30e80f0030fd70997d01dd611cbd213349e29b858971b2bafb456a5467050bc875da0df6163f138159431b0f08ca3bc2bab2e9cf46acc111c8b8f19be779291ef3123bb8087ed2228b955fa54edbbabb5fb0269b5121fcff3579cd5f7c9e8572e28240132f7af8837e7e13d124a3610cb8b8accc553616556732368e563bcde44141fb96033851fe3f398533cef4c9f79df5e504272e1927cb15c8f891cc826cc47411b3f5b08552212f6d96e3dbaa2d262a0c4895283cf65ca035185058eed53b74df690155b8dc62e41c056eb7e19ec50cc3f4230f30124f8e281d5675547112bc031bc9b9f679f80ccb545a7c82c97c814dcdfb3cf08c7d7c45df39e3db0f4519fbaf1f25dce68a4ddc05d30510ad6abc100dfbe9e55cc497803136ca165ea0ee2a7f1651f927dfa13316fe20496bf6fa2302589baf25b8de2d9d0324c6bbd096b44dd2446bd8b73a70fdea617749a0faa4d2d1431e67b1e352508f0342bdc39bf38391f10a8d8e70261a06c1749060ce3562d0139710d0fd266ea9fc62617c3b657b55d6678b6c704bdcb8f5996de9da26c1eb2277aa6a7c3c68407de49f484d26214f94873a3c47a2368933597176d5e862d454f7c286245d1b60f3cdf8729eb7d1fa3c6d4450c8809d63512b303693e9443e7df53c4deeace910cf90598d8d395541e4272477f80a3343296f88f7e9c59126859e266a30c173d5fb20a0a410a97b69811f30560f758f3d057941717dbe1861205a82900e1814190ab0ea0b2caada381eb4e5fbe26c899024e4524bfa74e84fb4ea3a2179be52d988bdc1c670526d94ae84a725595ea480589c842650f25cb5cb1a3366795074e7f58ec3d2bed24790f59fdf2e39232958d52b6bafcf8088b44ace7768de32be78d562e2e258d9e92c5ff468ddf0d0dd2723f377843cc7c0ea886300be833df39846909318894f1e6f0331516312c981ec643a30b8e1f6bc3f544577da27bee6114ada81c6b2bfd9dd819ca996d3e8d0985c8315aee13c0f9bab0e1a02ba432201dbff086e40a3c737208b8a0ec36cefffcf6e7a6c854dfcf1b8aebd4155e91ef5b9dcac0d871e017f16f185155e83b18ebabe7f8e574c3bce7ec0c0176e69863730ea5e30fd09a963d99016b9f93f8c6d81e2c2b3be9201c5ea3dad45dfe1974734048fd408b92d14484dc3aca2e39ae2ba8c40b8729227cd5dacdda200abee1215a2cb4525fe7c32f7cb06cbde92116a4df736eddac56cb33f28c7614460025b6a9fe52397105476fda25dea2f6862f73d082f9906d0d4fb5e967b7dca4895aa10a4dc5410b1244f77e33cf04ee10239f6127211facb37c8348c57e84e677010294f94d5c1e8bd29786bd7c7a838bece697caf3ba07b6aeccc5e3d17175d7a67f7de5886b7ca400e8cf50ad6a76b44262383a4923ecf054b7855ae75fe7eea9fc2f747004245a1e8bb990d656d0999bf944f9f7c73735f9e61ce4f557e548e08fc60e42a1ead0404fa6d56494e3e1ec2adcea2754842dc7579c5c083a8d4782f5925bd5fc31faec6d4b960f71dd9321157bd92bb5d349fca78177fa998a8c948c28dc00ef2b421f12fd878497d4211e99b95f97ef60b164defe99d73e6701e6999d70a975dcdb964850d7a515eae8aa6254183344c6f1e3f1a22f2118a7227223895fb7e422587f0b1e9b58b285b9d72b5a1894c8eab1f7dd352b17ab673dad7af95cddc1791aa8096677ee3a0ad04a5d2b42f57d5f72834349e859ecd762cbb660c3e7242f7602fb71532a36d157c2c1c1edac993ed40f5d104c373af3636c26d43103e27a8c3e98de1e2e9dc86cecf2dfa60ca480a8464e83e01e3d37281f0de51a64cd673577cd3cd1e98f69cfe77b2a4fccbeb3c257e907eb8ae3a1d9b6d4bb7c797fe8bd5ae0fcac5e03b36ffcf6122fa6fdc463676d89fbfa359246cb6441946e81672f01e9166fbaba9e6c3a387ccdc4ec004354014040c89bb74c7167a37faf95d6ef504a62e2ee1ae8c8232e1c2784b05c7ea54fc2f087fa7d710f8f367069ee2e3cafe512be97edf282dcf44d632d18ee790402cbc0bbe242b0f94e7835a40dbc6cabb094744afd37d7a8cd55e47e5ee4cc066fc96b32f2e69dfbe5fdfd2c956e82c9c72e6875d58f8f26f0bd2dbd63ae1f37de58ba81406ffea6e084b004dd7e4b74c7d6a59d2a4c86cd2b35a8aac4b2b11e365617ae44d7f8e5c40ad7465e99f0620f9b05070084f8c063e8f2344887a0982183e453e988ba8da8d84e2ae06ec2070665a95bf1d5cea8c6829d3f0e53d7602d43c61bfa72640dc560213b09152cea4527726b199eaba6993aedaa327fddbc257a06a13f13a69cf96a19cfb490c7b6dcd64af736c494b897cdaa21ce30f95de60d2cf9510c4a55d75497b748cc417c6757e980ba23b712ab31bad722ca3dc7f41596298bdebf3fe2a22daadd14f737e887346f7e96598dc088d34345d65f3acc86155cac7bf5f62d9c3e79e91afec9f82763aaccd09bd49f5326418a7e00f1d589d7ebd054644517e00d1af155720f0d77aa532d53a825018fa6a8ef0192b1ab2368bd0c5664d538860cb5970f3449b55f794a2158e739a7098124d98f016eab7a293c9f07d27dafcbe391f03bf1ef61797f53f3e48d78845c65c4ea60db89ffd9ae3fb9e7f1fc6e5c5e1beb59369d4521261a8b85c3cf330695e921febbe4ce88e05d9506d001612a6805178e10d499c6e4e2de6a9b3e636162e0a1b5b1835733dd667b64bd6a1f668b05afe943a7da8a3cb2f6a35fa2386d8aaa76af36b26402f58b56e87443511bceb77dea4ce68767407ba48df4b97ab6a4b3066edb92a2753544dc859a92158b549f835fb9cf3b2e36fce7f1093849a83acd97655d4c5bb674e1ba316860fbdea318a6784799673c3960310a299e253e0a2ed888a764d183ee3055c38c4c522758f2be7c9728515ad22cefae31726e9c09d62721b0148c8885cbd7911fb78c7f2504ea0fd39054b5fd9e343d434774861a86b957e1436c3454f098183a5eee2cc2342bc23ffc1c292048119794fa76cc887ce8b432996a8923d995d6e485fa62a821f9d52d3b3a92955baf1bda69ce4ed4f987e07cef771e063368743c8d67fa683f14bb58de4dfeb934620af6532e160c4d94238a65e61864d11985aab189b7c54a02e56cd11975393826a1408630daffe959be60a8612f609ed4439808b5e85bb5ab254f2934cecaae9dcec1933f1b94ff941c442eaea2aec2737379ddb0e8d940d1b978495616c44b6b7c093263fa01458514ab6b0df462ab11a7d235ea14ff151ef641dbabf3151ff3bb17851e43f920300d1c907c2e3b439429dc523a72412222c607bf110feaf1d27fc3280ce7ff90a6446d5ae3ad88b50d69eb2117588cd03009be1d54d344e3678055d70e5979fc5e859bb108af93803bb895bbdbf33d9be527f01fd09cc2ed5925663f57f7787d29fde83ac3a40ec81a689cc2692619ac3682cfb92c03120ea1fb4bc9022b4fd4483d150d12c50a60d097f4c059dfc9749b5fe379fd8e29dbd6f7c07ace55b22ed1c70f670fdd08a6e9d37e360fa8fba17595f8a8c2fcfafd0ec93214ec02158371d29e20c3ee08fe497ddaecb30bb03cef4294f2aba1094989cbe522ac513fd9c3bb082a2d0159c9583b74d60214c264712bbb693ccf74d3cc6fb1af3b284748e533766eb7aabb892aa386b5a227a0989da9d57b51be0ee82316b826f802662d5546f297b2093c98c03973df2a1bb3a0d6f09ae4589008b5f79ad51c150c8e59fb945d41cec1f0f22c099624dc513ceda98205951e11e9b6dc2ff4f5efaf8e4194ed7eaa576580e6ecb51d8c7906e5095ae4ec2d1763fbb5298684e1cc109cb9ec3d8f84fadf894a06ec56a827bc919106861257765b4a540a81807336365b52e110c2abbeb2bb26b229219b7e174e76f0df6f7eebf042e5b47aa786dd872c4323aeb9b98a6ae496cd00edf157514c380480367e4df4bec7f8dfe18e10024aea9905f97ef01f2d39f04c1dd142433a08670755c678ec13dc532a4fd88facfb58249c21059fe3c0cbdf3355eafd3529cc246cc1668432d07631022ee4614bdef01c29b02d47dd8638abf2973e01fba9b4cb654d5c8c5732b2b4a85ed56559cb73e519a844dd99ea3de3ac95ee9f15fd3f34a168bddd076dffcb00f63ed2322811f3320bcaeda46859aea2149fc9f2dccf80eafe4a8d57cf14265980a3eb52fee43033fa87821ffd2f9546363883762b1be4b39b8aebb1bfbf26698def9067bbacca795039b20ad64bfa31c5bcf5efc07f5f73c81a8b6bc11fc3a6657859c4c87fd4622f59b843df5cbd5abd7a2df044f38dca07424cbe1f87b15fd900312d453e3ef436b21c36c0b38a52eef0f363f3a37b918c5725653b5d3e3f4ec8d18d4d43856a34fe6cce54f6b98a0860902a21e5764c8073243174b1c18716f2ffd7de7b9929bbda94519444941a1dd665ea3729077474430f477636b1ff00fa4bba915ca2258854d82d32b20242549aa2296a2a462a1fd2bbe68499dd0fcbadad91493b131d85fa8fe010903e7f5716973e2e87b4ea91bc9f95fe1214c57c7ca4dcfdef446cbf9fab6f389ad177548317fbfea439960e9058962753205fae5e5739113d4efd34c47d5c95b992afb488a6a0c83de73a22f89f033a798d038c9c4de0a1ff06ff6755df273f73fe7a122771ed9a81e4eeac191e752fd721b7d6ea177d57a47e9345139af915ec0b59e4e34066daa08b047f27f1a8de86ed30e4bb5fdc304efd37b92c2a3c818d76bc45c255a697e699d33c9c9ac87c62928dafd33e493c6b291aa16bae52435491da70c4d97483ae7514226627d81c714d98eb32445681286241fcc79405b31b9d2b9ed9fc9de605e44d67b97b626a0d1ce53534fb4f1cc899862a246dd7e291a4053bee6afa1240fab1295de99d81aa58df7a5dfc6d0d64c16652d17e363f30624953e0a96caf9a4d9da4d6c4e4d9512b00d1526e728966b38f381dc56f40738b0acd89006ff7139c2a39401acfd73e2d63703252bd608f835a62ba790534de023db782a7ba7af402e19399485c4a1e523497cc68e9f78ef924ac195a13189a108cadce06a3c4e37cbb0469f48a0315e322d5221c9e574caf07b0d9263fa524c283181341b1aae117933bc535665ee63271254919116f042e481d10e5f60751776bc8ba335ff7e59dd1ba6150b584c63659550975fc82ffff7b5f0d011f550a6402085a20367bdcc0ee6f731e2346ea1f09368cc94f89f4fa24348af29a6eab230149e635165a08051118330dc63a8df8947d03307bf0de6ee7d26711d625b089059c9b24df725d06dbab970349719bdcd5aca37ef74711c14034e447266e62e3e92df1e4a4f28da70f166baf5cd20657172d423a6ccb25aef12acace78a11e5ba97d9512de8822cb2084aa1dcb3773d96b7797743deb5902ac29d97effbdb62b65abf4f4a801581e49e0fbfda8713bbfa2ff710056b2a8b738e1601df73b061ee1e7bcbfa3f4ad4f38a59b629d53b1505311916a515e13b343c5ed6ac6a4e7cc239273aceae8c4629496d3623b47cf47c3c70f8ef738325c36420592a72ee171ab59dcee915804d525f4a367762875942670993591c8a097043b80360df9abab7eb5b4aff174ebaeb3bd2637abd44985fb7000b50549848467d41a9d7c444ae331ac45772f1dac2eb65f7fa0e380a4d7590d6c6d0c041fd8ffde097d630f90b71117be2aada831e4416ed76836957e12da4682aa2147cc28cf6aba584ec8923ffd582f4c09a036e6859b99356b00f8ae3d4809921ab07d7d285e32a3ab0fd70cbf3ef02bd4a7c42a330b275092a93f66aebcb7c672ff6a6a79e67a2d532801e89484b3c6a0d4992cba2ad7c4f28d25b99c06237b547330961127fc250ff5803fef8a784a945fe057579bc41661a77e645d878a992072a343a87989f378ea31facfc8ef7a1dce57ac14c72b399482481df1151959ec4fa625e402d8427b210011d966fb23901eb675ccf7cbb50a5fc1b176827078ff3c37d6b5fd50ef50fc986630f98f0408020f63e3e5395c4686e3a544133557e0fbf0205344173fcec53af7f651f1bd5d07bdadbf0af9e98eb470e6afcad26a9969afc457187161ae4dc8558cf34afe425c6efb1cbd02780c55237123f89e1e73ed9e7aadbeb07056e42ff9f3207c90651a0a11bb56ca02eb96ea5cbf66f17ea8e6198f8518bf338613e7151a8c26e533bacc1355e7e262ebbcf3e194501a5ad2fb05fb931da975704342889dec6267231784a7cd4c9f25830a88174ec2389e5334998fca9f1fb7a3b7e0cd3c5545b7bf56d99d6ab263c5e1ed68d924396a32427be062696017d974d9d75d0228b2849fa6aee61602b3f5456d59fa2bfcb12e542add9417cf4461ac1b5c981f1a200100e0c22438bf76391924aa4fe68a93868a9e0e516884410a7eadc95dbbbe2d480b21ef9e5352e14ce76f675234f6c7f062cacd963f3fa988709cce1d6c4946fdfd005a230f623a7d593a5c5978d90dee04b88c1549dffc3b6179374e8bc2fc65168e1905234700703f0f1eea1e6e43d574bcf549c7d90a89b9f1736f4c98fdc667c0006f6ed01135583e910b089984b150a55190254a9a1851d8bbf342d9e66fae69b29aa0258f4c060d1b4a987fd14d847ff4c4a2782d58b359cf99eb18eab417043d6a9a92345ba48d05b3d247bf7d1f4f71cd9fa1c5b746cd75e069b14ebaca2e6811b716a668572d64e9dc35b350d47b66ef02ad8a715e69b6380d35748d2423d0fcfb01d9794ad0d144ec1439ff8f27a3f7fcfed22b4233da22a1b904e0e465e5869f10fafaa9beaf17c3e9fca9082d36b4c97176b8e6addf8fda66838f60d7a2a7fa11f16364d14d526d56d628131db94c6860a864abd057e6f2817436f5e4f4cc7eed2dbf4b168cb4f984b7b84a4841eb438d7b7c1e5b8eac8a96ceb6c58ad9b4a6a9300379a9e20bca0532a2506c63790a3580cc0fb1225b227e7a017db3d6d46abc2521d8449154b3d8c30f66015a95f499e53cc239792674d45b448065d867eb99d96666d6a1ab65e2e665213c3c1f32b6f38cb2ac54be05250dcebe4e00caf89b8f810792ec84115c504f2a861432d48c2e22d50620010b8135b7626c4e7fdaa09ab43c2ea9f76622d988777e02688ca1b8f5d81e770bd389f49cae4a80795a6d4fdb74bf8fd5a0ffdc1c0298c7161c4bad4199970353acdca773c5fc699070d6cf4d719cc0f124e103292b12feab27d31f5c42c0e5e400037e2c8db6faa5256ff5675e0a519762271ccd73d2d1f986dca25e8dccbe9d1fb6f2249c31411b35c1a2eb33ddf83dcffa836fc1706e973cfbfe6ec21ec632f2ae2cf48d05bafa17545f36c8c5d58465f0897d8116527691a5f9bdf1345dba8783cef696677aeb617e1a4dc04b71c55a8fad95cc5dadf3646f5257f098983d18dc4183f2a6d5d91e567118afbd70425e7bf27111b794fcae0d1862da8361d81f63f4da9be1fc9e58247a18fd92b1f745bc60b45ac455f2e6b70381dd4fd5aabdb9520ca85f6b8ac2a16483b1b2695d35ed80e68b6279491cf65ef8ea9c30cc3f012e9d4d8a08a743627fe07c920da7b8b560734b0764f81ab866845310cae2b01d7f6e561e5cf9f1b6eeb6c500ae6a12059326b2fba1f36a8ce5ebd6bfc616d1bbd3f3338f499caa0efdeeb4ac270afea11d8a2508a204336726234a6aba5f05793749984aa02af95d6f3ee950d2a5ec6316a7508bae2116aa5693c75263223bc479a02fbd1baf056c350f7fc14e5e4ebe3d6aa40e0c6d436365d2be866524a8ca168ce77bdf0b27e139a17d48e0dcffde29b23dfdeb312c735252de9b7635a08d7086a807443c79720a5de3d36c3218414f33f651c708f2f258a165acf55027c8167d7f5f904f83ea0425b77fc4f0c80f2cd0dfc2f825da0d5f1d02a8a63396db72f977dbe624b4b0d6947ff5a8a544a5419d3dbdad6740312f3e909bd7508d5837de627a0953189e76dddd9ec0314e7bd095af6494ddccd5175b904c88a216deb8410f626b6e7e7c44a3f6ea2dec334f811b6d23568a919d19ab266b8e3faa5ea7ef0aacd53ed8828e1a104164960fa11653a9b6a113c847277e6745f6c539943177e40b11d2b62d328558514403840a1653da46de9b0cb129160720d6918e5899315aa97e36df71923abc38902a24526c3847cc78f25e0e04c421ee0bb8730bd25378d35d084d72094e70435787bc09c904ed725d259112208773f38ca66807653e1ff70fa43a40c1d7cd6bc8b43b3d31a56ac900e11ce183f76d60cf57de82e2022f19746bdb003d14aa50596f6c505c97667e8da10bab1b0068e34f83d4bfe720563e2777f369ec993f9efcdddce441e9fa9c12c838435b7fdcbdf85f2d5f95312d1c3981b436bf882f69bfda5a548e2a1e3419bc6e211a82ffcd47fcc7edddac9234d72226b934b7b432a3c9eb136130e9148b2ea448d853b3ef1ba516d1fc0ce2140c476bb426925e6af7df7358b1535b1efe5b15f639bff74a25eaa1b5fc92acc6ec9f6794c3f21cc2268f3ae00bcecba1f0ba2772eb240eb5fc2cf3c1ec93255bac086f1793c4de3b09ad7ad987b60d5472c89feb47499f61d84d28776153db9fb85b726e5f5c320936ecbd91d68c2213bb64f21e284976efa7616a765e33d8170a58efa6ee52b6df83ce1a17b3e113108d0e4fa2f25d583b80648a2200806554ab59e23cdd80aab18b22cda137871d2321b34a945f0f89f3a90ed1204a5dcc0f87b9dba2e07f102672ad0bf1172018a987c2301994bd3953ccfd56d09efa3cb9c5464636f55491be1287b07bf299dbe07e9f3d14c0eaca48f3fc3f280715f9eb53d267951d85e7b69da01ecabb500c3b3d87d6734f75769120f1497204442cf69a28f4635152f56d1747a5dd1516e16916155a1b7277e3a33bd9a0092a045481a0e7c0e52d04b951ab57f52e1632a1d68b6237e54e629faeed39a3ddad5a318014a41571c071f4da72877059924b79f145f66db3b675c8b4fd24370ddc03b932acc813c481c87d96395e827c6cec99c81d385ed095971d16a37b936da061a7bc1ef7944aea2b877a97bbba495e2db7d646b26f25a675984871044e64a61079e73fd818a63a886856f30d845f5a16823fd34709b2edfcaa875c0870163bd83948d81c804eb1026018cabaa90e5b845f8066e148cd8056ebb523db48d0b41908261d7ea7a07fb80a78dc67b4dccdda3b8c5093862f7781254ad722befb26f4f8792045ee32cf23d3be096436d45760c8fe2196e611894cef55ba777aeac02e252c7b47dd27c68cbee39943283c6ef16d0aecaee81528576ebe86e8d683ee049f4758baa96bd0f1ee070f15005b2f2d1a0eef97fa644f2166908f77539e4977eed9dcd6477169e7c7258995de20ced167f5a3ea22313f9678f124b70e6f341530b64a981541db3685d8744bb78390e7d756721455ec4f9d3bded8ebdc853411ac118d82e448652a79370a5c201debee79ece7db8b631e68457e27457028771de0c4e4eb7d0029844f35f0f236771c7d02aaf7c44f47f9ed4d19923ebb1ba0f81d111f039ebe431c3cccd6718c306f9d2e59312e3512765ee5a272f68e104cb9d607ecd27ada2cf713bd01ccb84796fba2ef4f2c46f05ebb5ed6d2cf1ef921a0d477cf3b6611fecbf1ae8ef4d6365853bf87543960503b56558ad2e381a0d92feb0585b66b1dbc85f1b99d395fac2a3b0ee1501012524b880207beb31e3ad0ab5d767a405d37ce0c5119045f45e2f828e53ea76d96e88177b8821e99518efcb47f01b25749c587d53e96bced05ec3aa04ce03fbe6d6e14036b978e22c1177b0dd51ad6957baa22387feaf3101088e7471a4656fadc50d3300baf3b01ff748ffe8347471ded830e2440a0fc837d15daba67c873fba2a54e1961843e32ba3783ae786c2e041e32ca82c6026a8d7db9a683c6b21d853da30b7f7e92379e1e301b4905feb8f637be2ce4680f3b668fcea3bb451349e7e9b264116eaf8f033f467da883bd51e69f8e6bf8956d5080d879590f9368c86ee24cf9bdd1e408e1989b6bc0c776c77b1025e5300b5819b7745f11922b2c9af58b47f054e253d217638500867783f41677ce5614764f11362ed396875d0433d6d3d8c6699c03e15d917f9aea1e02eec141e2aaeefdf2a29b462f967296085ec82ac54779d29b3204236df01280624ddcd4b102f30e7e6c045e77abc12aa923c7173c86ccb7d5c6bb5a89360ac0ef7a3e30eb1c06e70513c028a471e133571456574218f85f56c3233f9be01719f0c06b938d6d80260e4216f82691991260b08860fc398a6275a35af51540a4858f09b9b75d727548c2431a7d6d3c96235fd0ec951206c4368520cda63ae04b47dd5a792e455d538ce8f7ef43f52ad40b2c9099bddb32334cc2b859ebd2c3cdd6c03cebba19411e3d1f0f3580f7cfc6997a79560a1478f6f9862613092a310561eb0a6b837f6d50f44d98656840ca7471a44dd2a68c9a5c17234952ade4fffc2e7d040938b4ac1bb15a384f0e8f2052dad544e441bdc2c26e35dc8a259419c6139f7ce32eb9fd8eab6826451104512bb2d66a1c10e0d10c5a9e42aae431a4af461e772e002cc51b73b166e04037e2209517cb51bc8b51ee12f0b876c9a0dc8aeea9f1c7dcd3a92f855a0f8554e49c699d2695ed1caffad388330ef1ee91935ca744cbe4a5d30f1de075b8a48feaac53da1eb84cf29b280b8cafcb5c4c01c793d65d7faa03d332ea7265898a9f59534e67f3d7e0b419174822b1b7cc1a4bd1a26e8a7fc400bc2db6d760e1725bfd7eb8db4cc59f937c618813e13f44a725b58238519eaf76d39ecaf8ee64d54f7838083d469d6797bac49eb0151e6f0f6d06255da010dc8a851aa311b6fd836621852e9245b78cddbf23b3c84dcf0b97ba0b9a76427ebb33565fc3606b48ac1c83874a9d8ce96e3539033c85e96080c65b907a54b1e356ff330a5af720728a5d1087599c4d43ba922ed5ac61dae760717fb8ac8cb158d9a8cbed8056373a5aed91ea2039177ff9b0b7bb6e6e918dab0084959a869a75c7b6500a397df8d383fa01b3be6ddfbdf744b36cd1a98c8c04c4b9e0fe518e2e02f2a6a4123ea77a70049ec221c7f62458fb628f15a51e3bd504949818ff487e36b8a11adba8537bc299cd2fd7fa172b9c60ac2d2329b061475c527df3ac972ea49becda0e05c8663520d170f93c711743b46e5bef6e331e0270c92bc991d1cc813846ed47d540585c8bdcd38a1d5fa5d68c7132d8d328de3c8b04b21030cca34c91e9c8baa467fd24ae7bd9e1ff6c839122c63ec85482e6521c46105a4a18d5f2b5ae88ef355480e54d9d25a74cec711d419d1dab20017a350e3741eb479c8acab72e4db47d3c6c9a92992cb16238fb09ad006cb6485eb46e22e803a877ddd0812f2d03aa2a4f4db9cfc54d52866c8b02d74c0ce1a60b5b9b47b2c24f29429c2e137a3c8e1f77368873ee308a0452edd1f45cefe1e50364479c2c97b53217b46931850faf000ff913ba47b0ad39e96dab09500ac275cb13d743f59cad4a26f0bdbfd80b438fec73f240498453af1333aa493ce3e35e37feb9b2573d3156c48f1fe04537e17a73918c58d1cebfcc5eb153b9295154f463eb7014c9d3e879c012694b1abfd32acb79c8a48c215d0da4654c955a0da8c9840eb19c9c8b9220142268b66785057c26b1d862ae67812831a057a93585ebae1e98b6973723199e2b9a5ae2b7c82f0f56b7da55f631161140821dd3f5209aba4ed4663d64eb4a59228335534715e8753ba6f5ca34836baf871bbb24bbf4366d891aa0a1df34b506543079b8c5f698e41f385f423aa4a3235b7a2aa8d0d9a5f524383c1f36b08b846b5e7596ac211d0555eb3fbc01e9607f9d7db33e2c9743e24fc5bc273943e31eea3e7a2c2154ff5935d18e372cca70e731e3fd3b54c6cb6e978681a9859f09bd74d430040b3b6c27966f57e3e3f716e15069c427f18c9f922f4208a1adb26f2586d0b1124b0bc8a56d92644282a604e32aac57ce859612d158bb954aacfd343306bc90248e47d61082d785c7954d6583b8d873232755e8dceda329a6efdb44c259c5ef48d99763dbf50ba5ff4aa02e2ad45d9a25a7887e98bdde214cc58111f79aa5861d1f87926dc7a456c4e7f6642f0b67ead0746bdc0cf1a6e3b44d72c8981b09667c032f7797d7eb4513585ffbb4e81d9b25dea5bed03511e388ed9cdd9a4a439c5edf899a299353317011491d78727f2b8f58a86ddacc112b93162b1bd6e49191f6dc2ce4f2a71fc7ee26545dbb1f54da6db21bb3297d100e4511cf459e8047379eaaf7703963d5e8bef72a98302bddb0690df7624252fcf5fbbe5c96748aae28a7fdf24f23f7b1d3df5a3192c60e4a6a3f53639614bd8c595cf5ecc622acf51886b1c6de57aa660a221468d27ca8b87676df8e49626741b62404fab8cf160dfcb6bce51dcebe8d208dd26f5a509b5400c55f8deb5f558b90e2dc01432180c00e17cd4c97907475ea83273f57c91c06921c1621abf7e7ea86929f3ede1f9d8233b68f7f4b5990f741c1287e4f606e2e4fde19dcea9ade2ced681ea50198430abd785876673350c98d5e11f58a551d6a7751059f20785916da5dd9bb74e5935e9da6c90b83267e2f47d120c9dcf70c088bcb6e47d8b98ed8518859982d41f1e016619b739bdcc0a7ca13bb5204c9fda217861b0012ca35a9373a0276117d7b9006b9b74af85a8fc04a0ff129acfd7a6d8ad37f030e8dc884c2a2c5f4740a61a21b35509aa5e61b6134f52d5b8050c86325d3728eb4ebc94e264a3c872c14c0bce2d72f8309a20c58e28f1f17e9379c12e5dccb32a2b0fa95e9be686fabd99f4838283728bc21bfaa1e4ee704df3c23fb4a523b6a07fb51e20fa42112f87d8863c66551f8a170e75a58acde32cf689ce61e8bc57799ce42a94e14eb8e6f2d17b37911f2f79c8ee7413969d3b705364412bb356fd5f288fda046be53655afe00c756d4a9956d37f52ca64afa064966af5290ab5ae3d68100472d296b63668aaf40ca005254f4b602a9d3126e7798a8067c9c7e6f1b52191a62626a992b05a6b05142730b2e8ff0fe335e0ac4ec6e6895f686742576bf71106500a36c77f55312e10a0cdb4443f2c05745a18d3a6c9424c2bf8baacd46445fc99a9823a982add7db9cdb2368984a6ece9cabbef41ab06d982bab196476a105ea894dd25538692ee5f070040dc142ccf5d6c2494d98463da8bc97a9b1ec6f70d18a775fb4e999fb7d6a1c27c6d674e7918de9f3371996e151d8622f43915c235b49681872bc055f925e5b94048221b1ce67c53933327827a886cff352028ea0e390e55423d643cccda03f90c82cad414e35a71a19f67c1451298a96db78a4179770acddfc0f3b1473ea39a9ba43f0436196821b1cb6b72a0f30406a33e0a1217fb553efc490a367bcfd2e23384e0fba495ffc72cf01e68bf738d2c87d525610fc2a6ef48ef48d1c9f6bf19affa12a3906631dba3e95e7b03630f8f92e0ef7e34bdf01fddc9a0ef082d254f0147147168b2e4286bc67611dca00d6e9faf5d0afc0f93400282157595c44ba2cc637459401a7dbcf7226c75617db70378202ee6f67b8892fcef3507fc94434597a4eb87778b17fe213b451822a5aed06f669daab350cbc876ab222afbaf11b3c0c80a4855bb8a238400d375b0b2340f1c8e5d195c7648f9c3a8d952a18c7c1fb5b38108f64145b52bc21001f3230df2e8fe8bf99f1a753743b8607899131afc913864810baf1917b8612560cc92604c513bb74578e04a7ab78aa5bdbbdd60428bae0a3188526da4e3fa64e30256a790bafb687f98bbdcb00d38fac6014507300b0f3448cb7a6f51bd784bcd872c406d19a8e6c0321bd94453dc919c99ec6ed636f3e3b92cda0e5dfb1eb51353feb1f923c7d4c65b03bf0452be3766184267a2450093086fe08a8daced7b22ee40583fec0a9b19134f0e89e687cc0ecc8eca2b6377f85535a73240e8b039c0905cd0fabf3ba454b0f3aedcadcbb0be2de2ae9e4d8da90281c69f10993510a23b895ca77c1a22e3b6b3d6bf6643df2c81c4fbec47e7db07540fe10fecd46a73395d9ad28facd344f45cc86586a3fd907ac071c6071701047e82307ff28ea984c5017cec33e40f53d24132f719c18d4ad63ecd6d0cffe93f20d0cd4417efe13911b035c8f8643ad1dc2bb9a58ee039aa08d8377a9d889f68c58caaa8d5deb652bb12b04b95f4432f857036cf0ba8e3b0d529a34a2fa542aa4da8907a1ef87cca28f6c84bef7c74b046c7c2b1bf3f0eb265bc94ae79e9da6554f4d6a14bd963c2839fbce33c1385d4f3ce1f7f9a53a86f26dfadd0b72381f492baaebe22970db7683425ec63086e6f9ff5e99c2b93d81ec29c4009dfefa10bc91c227db3afce27035306b97b10be119f1fa9378d851b7772da22acb23644863c77ec891cd5b2a02783831192cd78aabda2d4fa2eeeca487ef333977a26c4166b337f53ba9363044c5c7a177c7dc6a057c4d93dcfc22b779d98e01c54cba34294073fcab252c7586f5f3ed03ea94d34ff211b347a1edf648114ca4a108f9ddd6541e50152b7c408ab12464d61ae4fce69a308932323deec924ba28883f43d4b97876ff1bc1e0543e3f9b3d714f5a8bd32e6fffa2d5614e3b39a1253d632ec6066b62fc0eb7fb6fe1b8d3066870bb9289965bbf9733fb43780f94dcf26ced8947ab91c92821d57f3dd9a47b5b850e1a94272c2beba1200ba9718612102ac3b0dacba0ace908c20af7e10923c0ec149d2b7e4ae31d0df2922ad8cae050d4f69d5dbd6f1fe53b8481a87f447bce46807a284d861a331afd6da688c0c162f7103dbf1ab898506a386449e9552c2b30f169b9fb49b6f1921f9b6d6466e9f951851dc1a35c40e1117053ff103c2e6895966e9759d96af99af19be3563cb0502926e49817bbf1b85e541f4f0fa20c444c6f93af4e4c4331549ba6f29df1fdd02144419a8a2643d508bb7add3fe1d01a7e9d2456272cf53312c37f5668f4f4591cbb8f13a8d32382f612e6b7e14eac274a57b14cc3cb0e0ce604037b48c1621e87375014db491f3ce7ed5090ea401c2d5687cae4c32e4e178c72612f0a3d61a902a31599cc7f4aa6d16e1f42366c1fe7f1323e46190b2fa438718579721f55964218dc100543c4a32ed371fed45218168c68f5a4999a71368ce1bd4bb4f772995645ad50685dc2bbe1e3c9cfc8d314cf35bd9dec85dbfba8ed3c70c86453ac397ffc91b544aaba9121ac8d5937c45d79f0be2d80dfb3e1e0412bfd30b28dfbd36a6ff51f281b6bf56c0c76a734d35caf8959d7cc7c235281e387240a45062d2089fd8fb4633f1859595ec736e1d3298530b412bf049f291917b13803c499b537538bbd4851fe7f244a5921ad3323cc1e12848662d84c0789beb85549f3ae5c07d14ac320960448ab35c2cc2ad0a6074f012dfdf17714ac5def87abc9a2e49ffe9c49725fe9c5d22d3b85b8836d0ef300b261d6e628c04cb60d303badccadd28c1785a6b56d26902d6e918642a301bd01d2c67fc98dd1c4eb23a061ee393dea1e179e2270269fb56c423416447a12ffad3cc3ecd0bd1fbfb67d7dba5d2516f5d27c65e5fdf2a1395c6214ff6755cec12d0fd8f2f9795e0de8e2833721ce1e912b9dfdd89187b7d7c6042932196b81f632effc4ba53c3e617783a3c62ff35653a1ed678dd4e8a30afaffcf5cad654195ada6d9fdac3b06b34d0555d4395fe5403ab2b73f2615165baa0970bcb49bb90943783e5d078cbb89b9025c6d92c426532dfef100215fa8e6edc05b139db7f86321f6e423a6de5111e9f34a840e5fcc4e9521994063291e4f26b693ab717112baaff72cd2b84a9c1896364ac172d6304d53decb191ae87b959b088a74adad3f75ab312b63fab11e1bdfd8f1cd6def0a9715973cce63b7260ff8feec497c1f22cf1603087e24991066b4297a9affc9e053182f7b08ef3c8ef0356e4be520158bc679394828257fe0ff5fdda74f52996a8b6a85427eb2c01a9badc5e28fafe8977019f404ba80b8b78de996e0ce1ca569b57405e157cf017b5e3fde27a4512da0370223c663e312c955cca2ab6880014e330383464ca332926a3f0759e1dc779aab1c5e824a349c22fd0cbb322b35cabb537ec6e0df70cae582b3145cd3d9daa48ef31be1f4cdf39589e376ec8c5aece8a3691c6573318bf752f7be4ca419b9d1c4c77f4a837a4dc36a4b5076b1975c02942c497de26355061ccb8e922cad28d72aefc69bdfcbc94228373fe350b1d00d22217818b4495cf63b166a1fbd606f6743e7ffbb2f982edf723a45f83955f7c8755e8531bd4dec675afae5eb667a76479930506569ebf331aeeae22996b74e9bf4ee38f4972bca9faa487485a393dfc7ca8f9b9d07575e1c6b53aac277dbe068988a889c2de14c2da1866aa031f143a854f5c43bbe8e3e27d7cfdf530637a01a96abb424f135434ae225e895dd7553217f883f233c1b521c618db4caf2fc9d547566e0c8ffc7aa5435a0d38c0e04b5e6a9ecb3bc3a844181d73da3837f5422ffb68d1848a9df404de7754cb4069323c42bd5e975e6f21d7e3bb0ede1b495ae2a20059430c1310189aa7ef8608c8609238733ac8327f8c2818221b6c82d2711ce768a4aecbf3be76cb236b2d2af143f382086d8635e3b398a1cb45f9961bdd45be4b546716bfc9e167d7b37d1c2efc642dc95308a29318a07bd1a8a27f073833cbbba939cb29048718996462f120ae2f516505e4aef20a3d42e6c7f7f1ee5cf43ee734701fc02e7e19aaf69eae960a61d499aca8d7fe3a71a9795f46dd261ce7daa3948467b611a2f6024d7f17ccad7c5d3165e59692607674a23d768609dc2c833d0c4a270801639413f1b73fb91bc83f1b99b2ba57213ab2c006047a19f0d42a568702a37ec64774cd37c27eef3bd0cca3692723fd45783f73db61593407b257ba2bf3d3a3c312c64d0343eca8f01f511e0cfa57e8bd11f3885d1e714b41373d4bec241403779b578d081ef26e55d00474ff2cbdbf2818160d9052326426d1ab128d45a85592570ac752876f0b29621d0258df830f31e0da386dc01f8802c5e7d024c1bf28b87d9486df3ae49dd1d2319f6e1f6942ff06e01109f0dc965f189d95278f7a78c7c03aa58e478f4146f0f28729f144dd9d542be5a99d186d114e9f51e571ba11408e43b31621b59429c86463e55245863d6606a7f592fbe77ca701b75806c7c39418bfa496e25078ce08344d44019429e45a7cb2466915587fc051344751621a2a8bfc5091fea77afc333ced9e49717319aca6144c4fbf6b8d7c20d9513c1deedf2bcc3200fbec0a0ab970110d4261982e0e7eae8ef50b3b61163193b8e532c8001617dbcb747f71ad8a7f47c41cc9a6c48cb7cc54c5e39eaa6d8b5cf24206ac44c4a754172184da98d4dcc93457fb31a5476b2db5acd12bf090da61ec55ae11234bda11e141703dc2a1fe6c51ca62a0b2cf5c5468ca056906326a360085c709ab38f0cca96ae506c9213c17fc517ae79fe1c99763ee057709b16730f4fc8f78aa11752e4f070bff8e6d1966377623030c4fb4c362420d59a13dcbbbe5b6b4b70c2b435888bf48d0e1172bc8e34f8cde4fa4099677f0e1fba230382fa3b5a78c7be4dbd4be032ea452258846d26c7374628b0dfad371c94e37132d3ebd468ec8e262c737d90d0276e9324fd2006be05a678727272b1fb4b47531b7c3403c885baaf24edb34b7e80f2a3553ab648a1ead457c983397950227ac33f0f93c9e28da368da82faa3512b349b4902aac17f16c6bdd0a2b1e9f2ed53a872f4476967901f20035b8712d697709f2848cea0aab28d52d9a80f8f3e3a1391bf6790fe974df8b18931d755e0c21d98177522520fd5596c31a1d82c12d9fc4fea8d894439246805022e2fc127c1d5c2b9143cadb437ecc222c83c23ad33c42d49ee9c4e69c523b474f72341e395bef627b6adf3f2fe190c670036d7229e0c21350fa44e0d0877f5d34f38c66d70c1ae8e26a21ea3d3202aef8899394d26b27a660b41a7e52e729f14a3d9e65c073520079c0ee03a82ef4c9542e20f9020e3239ce0a4905a287210c611e2eae2627b2c432a65b4dd25b5c90ded225764370cdeb3c9ff3738f47a08d1ecd032d21555e5fc162dfbaff451847ba98da21ce9ab671c1f849e03fa9bb153e84116222d64f5bcf729c1debd784a1994171c1c74ea13fbb5c56f392da02a59c8eea841bb431909b8ae0698f011114fe54ad479796d4d13bc326f6aaf7d916f6d3881961578906f454b0d9da2d647c105713653c3058512a2b548499a68fe696fa9beee77f9e11925881a35279f56287a1fe50d8f45e2f5853e17aae9c2539d7caa8e8fe837a98860e002d5be3a4cc525b42c87508274d1b808e2196aa2096dbc6900cfe435d747b6fb8ef4a1815d26825c6a0946c0441c673861798f1cb6e364909fa5b2de49c937172540b701d1d83a6e7146401c2e5b60b8b75203bb8905c7a9df38b6a8cc87bed05010beebd88dee319391b03f56131b2ee2b53f6edcc3a381210bcd0ed0a474de0a23b27c989f33f932ed343d6c7199892a6896f9d6419c989ffbebecbeb5498e9091c65fc39519e43ea74c551b5d9f2150401b8ee441fe80cb07bb2dc0bf366a59d473d960321be062866d3cb673d62d9dbde6062ed71d22b841688129161c203cd5404aef0fd7bb8ad14cc99448a7e90922a037873a97de5dd90c2aef9acf7cbf1c89f1cc68711528d03bfb40e0685981de7f0465faa2aadadc2bacc032be3f9d96015fb764f0cc8f5ac50fe60b206454ee2f30baf22116279452f7842c4df8cf4472be437fe4b72284af08499c94ca37a8673861ec5c8a63562856e26e549fab5765a9e48366e2ed9e1b23b1b8254ec0184b7b2725adaee54edb1f2f40f469ae09c7cf4580e7c111e40a0be55523c831488b2eb0266f42344613aee3f0e8e607b6dce9835a220e89dbe89c601722a975a23fea2eda55f49ef014f9875a03887f001e1035df7ac62d21ef911239ebaef469b43e75614166a3dab485c6b17ba04bf13018486bffa7cfffe742a734ca48517ca585650b4ae27b80db86eb6dd9c7661f385db6196c3c45a9d636c52db9cfc519d61935b1a005c4a869f078c550ccb7c3fdbe61c62b1f7eb111c0bad6f8fbefb64dec14009f1937048f3e940a89b50374b0611e48c076036225c1cfdd669c69740e355054a39171b877db07405427a481953124274b462bac2ceb1e0b05a3c48ddfb6e1c0792c59334ecce0afc193cbf4c789c994a260953a7b1080a233dee272283656b34ae43ff4dc85f3b45e9226a52b33dbb0c91a00556fdbb5b3dace632c73a93426e0b907c8eb3114a379af84c17781f111faf599c981c1f65a16998d5417f3597380e58293ca6274ceaa49c77dd227996d02406d787eb204848155aafa9b147b4f6592c891d4e40f8b79671b999d773e4e616769dc3763bc0f2f6a1271a897cda94bd492422fbd2d8fb9a8eb374f0d29ea8dd8e855b3c30185dbea69f50ddc6357f8d7bbd97865c69197590e0a6d905d9fcf0e7cdd9edad1ba618eda02d416afdedf33be4e63012cbaeb31102e1df0128ae0c9c9dd14b3d4ff390cc8d0f22c665adade5cf48adf3b5b74f498129611fea0668e889bba1b5a00d0e88e0e775d488e2163e398672b4cc640cd8dd18c89115af51182311fa82dcb8066331d29df57e76993db46a22675c5450976e029e36b3ca332c374d2756598687e77987a19ec9fc3ffcebc43c96b382ca44f04a3be09f56557deb6ff7e8d8a70d966da31e5a2d200fcd3b772475bafd8fe029c9ac79e9f2dc58cb115e87379a39385b45147e2710a6615860b79c6cbe2e334ef48c027cdabba4113d56f76932be42718f9f2538d9fda6cd553828d8fb1f1011dc2bd28fdb8aed52951178eff3d3da990bc99a63c6b01277a0a86cf49bed80aa73a2a28527e81ed810789d1daae660292f47fc13c158bf28ca1a5e445910ede7d964f1be1090ed7d72da52cc8584b82776db3201722dc0eff6cbdc49bf656e89722e05015831fe295d82690bc44dca03f4167945c9b2115c922c6d3be94c9230532d527174955211690730ebff2faa2559209ae63cd9ae8b5142b971f553d7069054ea2e20ccf7c7a40fa486ca663df18263a2726c0c0e9ef6df4d591d66edfbde3135d73097783773c0051ebbf709877283cc3dd67a23af6a703217e4c431ee60f10b42272695909e1f1de8646994a6692801b0dd2bba5e4b6f8bcf87baef36768e5925057e9e0a26bb6bfffa6db49fe6ab4502b674a40ecf15038c5fb46fec3c98e342054476dc81a42f03a6e56611f7bfbb017f33c4432a0c602ad4984b9fd371ade266ecc3d0440f43f188d91035042f3047357aebe60793f1df7e0c36c0c415a4218558283d845d962c9c83d8caf6513d2985f3900efa998e26dc745c8769589868183af0e797d6da3a3eb126611d6349b2d340dd2b6f94f53084d16269e11598fc92e65884eadadbe2b9af3dc720cf1c2c10ab8ca47bc4d611d1cdf7ddf83f50b6b865a252fb4ab50f11c8cdb5fa89a6782e946b2070a0a111c72cc859896d716f065f0077967a0089c808d837a27d2ed08e9a1e3fd5195ef3cd96ac73b778adf7688bc3e72778074400b32482f7b809b69b23f0ad3633133ca6cde37373f814464c82ec7f00b3fcbdc8155ff75dc3e396a4adb9c685a283d373d7b1d30f84e2d73ddb2493957944bec92b3dde09de16c84944bee5e7c55af3726b32c081c4c698dbc630c900c49706a4334c793511bed61129107e134c1b740f63ae63b1fc87af3d16face57e4e43dbd1507dfe9c9376b51f020b022e59bcd22810ecbe0cbb0c5b8cbf6a0683775a5198b7632cda2bb89c09e92da4cc69006657b27b58e2498daca260ad7e5f3bafbb0427a0349284501d23171fc13f818f24a824906d9e225e47958b809e1ae1d6e05e232395008d2fd06b713320464892abf47d47bf7b8b432e5ba1ac9477832a46bfe92d5d4ca7a20b3a4d80437ac35f3cb073a5cb4763ae120dc0c0c46e8bf75ebeb462a7caa53eeef59c4bd2bb4eb9d34866723bdfb1e15e2c543c75c9e2efabb5d049a08c0da5316bfa8a0a6a921fd022e2b15707ff027d33a85b129790fc447aa15c53ad24f864fd26b65b754472ce8756d991eecb35d97cf81f6ac565d0ccf704092da7c42b8bbb9ce21cba24c3d1180e4f8967efe198f8781d75ee35a4b0fef04f201b520380367c7b1d7192ad7bd551840e66a55cd7f7f2baa079ee9873cb6fcbc574a9c649d9707d1b4e2f7e2afd6d8d8386c61fd056bbd1ebb4061ce560f7d1b1ab217357acb9cec43f7b21db075e098158d5ccf06a7ed75d3a0af2a3e8ce01e6402748642b441cffa27a35d96fe48adcfba2afcfd3855fc18237137ce4350bdef918ece45e732f6b1e83e17fd173b3afd1905b8de0cb05a3ee783022d57a601822671ec70f06001f76f63bf191ba6ee2e510c69934ad03a9c20d52118909f863b1d043096281969ee9c9ab8ee93bbe89628b0e113f90434b58739adb78decc6f53a716637c1d7506e7669484260d4345d93a929588d5d6aa2e9940c3406ece67991cc7a0392ae26bd17dec2fee469beec383acf96330faddd2e2e3567467947f61870bc81fca832c49833e616eff92621118ebbcd386437e23975be1b88121feae1f939b97062a33b9d2bf17d86915b0519fba2acee0994751f9073c50a1252915fd369d48b0d4c917ab00755df41876d4b0f77ad4280b10cc8b8d4c826222582726a660850f3d00cb08dba61fdbb931712666acd72824de3dd11dd894d517aba661c94597767a756e632b5c12937138d89e9fd21ba5bdeff5e9b80faab631572119d4cb8b02fe231835402af2e527988c4e7735c4fdf1103b5aba52f4edcd5651e371b74bea34f315b1f3fd3cd27361fb78af1d027bc22166500c29846cfab94766877202a31e5c7c488e8a944ae393eb13faf8171aa733c422ae18df01d07f381f69e320e0cfe0a76ad10b351f1d3333d0608a829fda18f316c29dc867a76840914c1103c521fde6cca7738461a2f1176cea0717decf0dc217d612664ed60fdb01fb5c5b0739225ad92bdd6463c6a239b25c5ba7d9ce1cc73aa40b58e433f6c1af5907cd1ba862011759a7109ca328d1cad50adc0f2b0553626c66f86fcd91520f231013042c96da75958f142bf4bae7956a3a846d071d354a742bb8600fbde3fdd5f02fed6bcc40fbb62b6e299c4d9f02c37e0e7c5148aa77e8ad63ed8fb74e3f4d3603007c294abf517d4d3d595bcb12b1c5fee19d9a70a27de16901a799ae52b1a608ee11db7f8cd727adf4fcc0257d2dc43c61197d41156f1ad2bb9b92844ccdbd0d0722a961114c0f965eb4520c9fbf8a41556068bda940f0a8e84d8d0d314acd7b2fc5752e18cdb8fc99bb0e63b8206a9604779e82a0c70b34e8dae77a3a0a0784a106107f5343faf4f0eb1bbc38014e636010a6f5d7656b0b92b93cccb940ec821c6030fe5070c2cfaa65e4400cffbc3324fb3f0922cb534e772c9786b776600cdc0f44bf13a94b3ebd7ba34d99a8da99c7aa474d9040e15cd0dde0600981abd5a6f1fc5e4930234d695ab1d5112e4fd755c51d5fcc31971280954e70d67ed048a135884f7b52f6344aa825b5133b3d6366b568b24fba40fd831748577519780bdd8e3c2e72dcba9eab2c463a4d265e05af6a6ce6ab6bf121efb1989505c7c69eb671c3a9e84c9e4717d8e50d5ff0bbee9ec508df52a130d82dfce91a5dcadd93ed082f34fe9a69c7c2d9d7f21c74abe42ea69eacc75391c6018e903fda271cbf143c232c8d00608af910c6af21632dc1ed33e9233b151bcc5f5bf0da0b5c4c43dd30fb7748c8414f7c69b116a46aee4d92cf793e7f48fb10e5b4668f1341ffc91b9b5a38e61076eb1998c66c626dc69d77bf0bc231c760dbe9f2b7c0bbdc05fae9343d27ec1d488e30b06c732838b22ab1317346bb4c5288fd0e855a649ef76f129e90370933cc3daadda51dd7b330f9681fe4913bd0d73fdc4d26fd900d7152043de48cb1ae843bcf2efc8689566d1dc3e8532e0fdfda7cfc2628b7349f668474d6f4f9f0faac689ceb309612d493f829f378883ba432c450e6952918bc33b6c5d8eab8b188ab7cc1f6b7550eaf7c6c897ef0e33c4fd4caa65621054857283ca2f6a0b99bd6deae77040231042ba0c7b839501f291d8e16c59a186f69fc877656381fe83f277bfcff14ed15c3ff473fda79b63584cdd5954f9efab6f1119b97018a6d0de09f874c40a9e7be008f05b288f0ff147222beea0803076bbb43973ae428a8ae7b5e16db7c6577077ed94460d82327c7a3a48ae237563e9c2c6f353ca567d43e9a0208a9d6bf8a42094f67e64561a877d6e4452b511c83c82331972f9e55a80b3b06919dfea57a516ff551d19202316ca8b0358c0b87e15b885c84052b1df11696e4b1ca73a7a1174444031462049e0ffd8be4bb53c6309eeafeb898e1ed7f16028ffe1d632604f44517367d7e342b7c64205378bee70f8e85f11cf44d1a83ce7bfcc30183faecce0da567a1663f89789a8753e3c871523b7e9bbafb414a57a9f47d6314f25b74c5093e929c5af2f06438efaafc248bf3bdfc10c37a53405e8c27e80f3902b26b0ca196d396a006d2b9acd4a4de5235d12d795ef394a96e2a6aa2b568bf28c0f399d5fb581510eddca49e1fe836dcd82d2b9795e1bfa3a0aaceed6b3d3b1179ce703e65b238d02af520527b5815ddd88d63bce040c87634c4dbbc6800a529d0430d60af7721bbfb13ea954dece65af024f9e08a7b96b94ef32ba6f172269d6eb8b9896913c6a9b09a0d8616a2ced536ad41419149104702f86dbb829441a6ce29e91529be26a4102f5f1c4a9b785328cc47de23ed1bbf7257a034887685688c76becc5ad0a43bdeb3da05b7d06821f10c64706855524738089a2eb5173c0ccd18ccf325c25429262b551fca6ff9f4d6aa64ad0698bf468992a2089a11a823bddb2c272e352b00a1d3bdb93c26c7019107fed852d36ad6bb8478a2476d027ac8629cf88b2fff9ecfc7fd53ecaf11ec585d86db195d9fc2b972bb23c827320e87045eb9ff506d1d418944dac8d9dbc4cc92a58bf135083e796e3090afaafced7e7a0e04feafebbf30c5b67af55afb47ca3d91d001498c48bc5a36d3a374d30b6d5cb1e536cee3a9766d8e43612338210e8e9cde9e45d4c2e59c0f2fd2e631f97118ac2f2d14fbfce10d3cc300826858ed3a2fa7f8e54f4d7333635f64701844d43addd5786463bb642be02bd852397bc10178f5873b24f15d26c9bedf6c5c72183c2a64ffa63621c3dca7ed5a5e4d2a6703f435f5dba33afe2afd7ae1860f31ef7ef92d1fe4c3a951414466cac5761639d9abee6a7c4c3b20ac4747840e8116a597e574719951deef7d294d5571ab3ecca4541b758352dec78cf35b5ed5f68975b50691bca7697db49638140ee19716ccee323d769be0ee5ca42e59f83ae3f295fe7041afd11664fac5a7ccd8c272792eeb63ed52db8264777149bd790fe7195b644bcee79cd4f1c2c223330a899caccc5ca1838af8bf2cd3c6c5d072f1676d92b866cb9fd9c05fa4b27eb420904d831788979c7891d9c375291bf7eab719835b6aaa17e1ab93efe7c0e69d31756d765c3e52b313273a2e2c40fe84d348dcc9b62342a484769a017979d78357606bafa7cdfc3c1b71a2a52dc0f1e6d38895bd86718ed2836c679787988358d01dfeca3615e61341ce155bc0c856a4c9138d20427d86eddbe980cb406023245497151dfa49f4af8132817e6533da0ed9716f095b51fedfb343b73990b7cc44fe388228b851129fa6773960b5c7d06d3e6e11dceda3f10271ca77528b720e8961305164a5cc203f4907090e9923e87b67b2867fbe59d5b510c4a8f5bcf25219112f2d005de6d8c2e769fda967fb1a1407909a43e369b651cc2e9f4bb9044f7b771971544a64120a65103cc92be42d461e07accc5f8c4b1fca84c8bff3b164efea1f482c2f6bccd5f737d547f768f003f06d7eba681387c495cccc0f33f704c32d04525e46e156d5dad8e6d37d5725d7fb0fc7b5b8148bf3c7b5cf6d721304090a693b865d616179b1177ecc2524f3c9eede3f2f36e708d637b071833f4630de6a0245078f0089317f4e1c77dac4f714222846d955f02b3a99ced8ef09e7b0298a9d0e4e475af862b99e2fb74574c9b94ff473989c002ae288715d84719584dc326de68563b4a44839dcc642b830c14ef35b8784f090486ba0132b35f7d2aa1d8ef09066055dafe1921f19eddd638370456af796c13a381c448627aa8616226d98a1dee16cb112e28b76a1fb2576b244db8de0ba2f8f2924fd3890383d072144413bcb84331b433329c0df5d9fe9dd8cf126225badac842ce5a230749370b21d5be39b219ba4b05f777050a8c1580daea509975a55454151edb884e1c03b96adbc2ddc8b55c5e8804bebfe78056b3cdb3fb29f9519ce896ce8dc718dedcb3a6f4834754d8e0e48c1ad29d485ceff227f391a22e49aaacd08c25c3bfce761a3be53d11365e923ed7409f9296ea93740448153599020b10b5c3bf6a4ae61e83cf9d34c5059ea296c53760730a2989e9c9a932c54dc779ef433b3ecd075ed0300aa225aa7617c22c901851be7d9f7d36a75f518887923358955c5a05190d6ab9df4d394ce17f5d6627768128e3dc5cd6e61f1543bc7f8b4d06b40a4f0c3537558909bdbd87216769d86f58d2f12e885064741ff28db3fc935a264ce7f5d3ff9f612a1dabc632bf202ddfa2ab7734b371f2a17437ca4d575ff796ebf5135dc1f8bfdaf0f6ea7b1605cf0419121d3210404f48c7686e3c44b1fb3e735000aa8433ceb376ace53b7e6c3bccef5e99b13f888aaa5b748bbb5fb254e1e353bf3513f1f284a64ddafd82ee0b4dc64df11a7272731458af551d076f8e11ddd0e283f72fc28e48b2319b5a81f8b7225910effbd27c61e8e4e58a8b7eddd6fed0a0e2407bd51cf7d875f22e8d08f90342399fb04e4ed2e7186b9199f6c11c1063407135468b770ac3d4d93036006d5f9659d9cf1edcf525decc8cf1bfd74cf8ac6ec0d76359e1ce7120867e6ccb6db8fb97b650ef4fb3276119e2fa24864aa7d32e347aefb56b6aac2c4c5e932022799494a6c189602ad0febde1d4976dfb1bb5c5683613066eb51508c3a41e64cfd7b0fd709606e70ea1efe1b8a3938949335eadc0eb1bc235da411801aac0b8f202a9efc741804e583ffc8731b8d97fa75bde439d7e9a4e2b3aba274527d6186a7af56083539f2247d9f564e857e014d1cc063bca18b8997ac044b489d61ec40faf6e931477e9b0f4d625506b7759d2efd25c4dcdb5815298baaac8940ef457adee0bf07c850f65cfe7ece5b47b4e783175561d131782784816d0f36b1209af7e894f88d94506a63460c026978d5abe471902388d9eb87b9b1a2ae2483d93292f655e30c6817445190b91ba805057b734ca071ac37e5a6072b71acba3b5247c3836a9ca41844a2f9d1e1cc9cab9fcbd8cbc8a8a62b9edf00c85f449c799eba73614385f3c080318374159cef364b0200074f15313aa77750e07741feef4645e5614b480c3675a27d98951b5bca520b2e3e4118e2b877017cad42d172720c52f83c5be64b2d31a5329c6248c48fa0cbbfea141b1500c0cd67c8cd04cf5ed9bfe6152dc54c68bacb7294451295cdf9bcfa387781102873f346b9ddf61d0657442cfc43b1c5a008bdb991bc283fbf0881e9a3f0fd68f677ed61d5cb6df0ef9b7978c3880ed922c736f42aa35a7c92e6ef45f350d68002ba2242d4903882ce9db49a0a9a80d1911560f97d9720529e6862d96360d895bb3328d65807dbc2863070b46a0e96e8a23f459848c37a6f34ce8c099a074e3a24ca50c58bbd10f966a226ef8d3d64b28a599153464f057b211925e6051c86edf32953ac4945e64a3f8db497aaae8adb874fc9948183d681e0dfbd20a8d06d7c9fa3ed56e1885d8c462e987926e0d4c3a962c2fb21a5a3be599f21bd6dad55f7e987a1b1b96361fccab147c9d28b13f5c434f81904eb2e25d1148666977c735afa3accc03660bc2a86f4714b9736bbe7a688b6fb45c06af2b746692ef32fea97666e69b4f753eb98cf839e5981c6e5567c43627315b2039bca3fcbaab73982803ef4d8c4195e52e602d6b85beced2e93b18d29ca0cb6baba4fd59210c2a23feaa872d8ff36634e48a0db999fafc7169fcb9ee1b5e3724e08c77b0810a019e29cbbc9fee423514192c336caaa30d9842383a38cdacd38b5b40c6708b6447bb285325cfb19c61145b9bc4e4a62464ec45650c1e67e9a5b8137ddfe6126b2495ba060861558608c19f49d4c6f02d7ee9f703feb3ab9fa911cb53799f316da19d115d86e4b22a7f32e4f0ffbd816e257a765daf21785ead42f6597bb0e65cdbc22227376f8efbe8b21b931a0d73af1fe1b747fee14694f87dad91178193278809fc24b8802e189105658e4a7aa0ba6e1bae22b7887cc72e4c85c94fc94c3be93a98bd5341c8b3fb0a92e9b82ce55e1d0bd4de4f83e62f599898b861e11ea47677abd2e3055c204869c3618bc2a3b685d35d1d52343f57c53442715392a5bef36602703a40439e98916e2d0ce355d766df2f4ff1bcd6bdcb641b422e6db9b1610bb006b077cc8368fa89d5588b857ff005c0c8bf6e5ef246ddabb4dc8fd4b0760848c7e6958907f586aa3dfbfab989d1375907e313f68e9734a257123d5576cf2eee4ca11f0d27cc5183b7ede5b41b39a5ea62dec80fcea6cbc110f31a0e5f47c39099f69cc7c0572f4930aa388b6a07597a133fb8dd8a30f459a8426128fbc4723ccf3e8c9ed83adc2ecd1e653eb8acf729e732db33aab00c89cd591f34742fcb880e8dafc9129a388eede1c6cdac611b0dc74cacb9b2b1df012fca01242d0ac8c7ccae79170493e4e65c2dbcacccc9553f9e54d7d7796f764f3bdd68c7cec3810d15d08bbe215a5ec1cf20471ce4ba5b0eccddc0a83e09eca40946d6bf49e1b3a05185d53b1b4d54372d447bf748445f54f9d416fbe9a593b29391dc9789c48997569c6d4c412ca29992d3b322c31f0cd0161f62630de227d96d25ef239a379b10f0ca46036559062b8799013f55f9f924ac6172a53b0b62b4bd016ab061b371863c52e10ea576c56872c10c008365fe3dbddcd8956ac35ac115bb60fd63275560660fbdd0c42aed0818ddca5763eebbd697f143462e0d19de2f888aa1a5951031ac0251feaba78bd82d876071f70dc04fcc3e58de1a18e1d67ab120676301134ba14db4908b45be66bdf22b5e54701d0c8f9a16bb4504c8d6e3c833c162b584c12b55a66c53fb33740ed098edc4797e24e77d5dd7ce6839c30fb5eda202cac2a96a7b4fbdd864bffb3fa3c899405182a332f849546a2ce856fdb023e695bd2bbac0aa1196049fcd9da2c330671cb2e16ffd15d11e92266a9114533c7c08f5d534f57c601ffcfecfd4a8e994b933ad7773dd6d68b37969ff5c564daa3dd823be8fd45f59c79dc51cc548ce9a20d73d17a23985da4beba22074b2a2eb3d0006e1e6ec0d7bd71d0c10b068f3c9a621afa4c17a6c52a05e855b90975c817a7307fde127bd9aa5c2f66c076e52434fb9a50369ae9ace1b3d7e52a8243bf5f60ffe2db3b82181fc13a265d5a6afbb05695bb8042f46f89d6df166d56e9e231fd2ccba7618b4514fc566355182577c2398fd2ae762102fae43972c7e78e179ba1aea3628a56371dd68e69225a594270b8b65edb65d19bce337a36bb300617985588bd93a8387f1b55b4397cbf783d0c827a41b228550785d9ecc5cd56625df9180ddc922d8d6f206fecce8fd8f564badd9f5f42334c6a8609458c47c51c76572026b9bd84c097c029e84f9d412bace8dbc328e72f178e67f660c2d245714008aa4cb73b673a8a5abb0fcc02f657e5fae7dd7e893c0fc34dcc87991b6df9f9c225387802d9afa058ff008a4bd8a15fe47a45be0267d3faa6baf575f4564b195cbcb6c97206df1525d0dc0e8ca88962092bc03ff569e4736f80639b7a2a1e198b76790bc5aa4e05792e05afc16e239682ce59284349a9ab736ef78c450f217764eaa8392e2a6ee9b6241315714532081277a26079ca46b08bce93798ac12a4bc0b032713d3dc348066b6d12b6ede9964d197fd0f6fdcb6476447417a894a5324af101a700555fa07b9aa41b3b9a5c2948496cb5b5cbfbde32488543031927fb2769fc670fdf45338f7ae1eb9a12783a95ce5473c5233c017acb238d198c655c3d4057b9dbd550bde050d73fb55d07d6f04f0b59d77731ab6619b519cae94cd3913ab6a05709deaee6a0df607e354c17c7e71ebe09706054f60539716f6753dc39332b9dc128b257b2127e86e033580000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000, Hex Digits = 47096; Dec Value = 199208119365537914170168129499445318340117842518688813204826638811074715008300249298099164510174237554063334311443395832703101143814837785123762304458652426869146876428714770986186757095037044820023543552481869989897667400270370657967265756963570724310685865056496234949904839917765752441192427344158151052389711865058194765811100565593397353046152347387739103723576808479549781321767947933811195766172434685542561500381133701297273910695284797420905439931659916491845989659466274049113137466197559029659072396983299012657701915907483299120476174544384373436955406142000608173808218289812376455355848803792143952242049452924191942121959565348775483465458083323635796849688098720606581646556606200570453949835911750235168542038675408617517017184793780734238787123245592122204336989516805302831213962231000239692431070013265316336332901623234752434310202024034855164487427928678370881239238177556808548641632329764085979586211441302300842977949473639697347497641553167835326935265239364233530400175237879450663077227300016126025102833960540013547777608530678978479822162043935367131477198952224171361150881467087039854010033049084274062332018455558712748112561925256698469571560887068141799553407751137190212887509006544798977554096631045607703272212063930053281091377785786858326065324140555295350181886220517123276486569989431997204836684953185924126578079278223864367948584645202455625006049151224518364135396687420695998167170738743415400268415887734713786012511241965225376697968120129735533508665351175246015978051293739642381085513644108280977556827577595244823730764304391979093259073911566913584332871404747450055712030656479330849081331853432052493839804971952123478440076859202818907725730535356023367000812603753354226082327667441750159249647015344266701378144696157553619455160482661783566722346064130199700267570580997418107591264423251364494523316096497060618052033070122866430968740975115362736518352871900678996640169794670944753487731559785884189650661516544711864589501038749478227804959876583942056892327498294977065078436415927479106471522311231812183319424492936015902479765691686768610300557749454925445639247208111267767676727926119822537138447124188615286001518335330274115286803829511440140606351607132264976993102474281329556812991091240204414990164346207092049779321794442966469359814222097116844128488223645627595492293230392849369793953008745940055607316755273831325084376871995371688149238600049275918959582079593252545641235352455410906246642728483045172416817928650930443412806646668023876809219160022224556324125608261716173113998732087397036138881663776499176544492332367019042663244972664736733163206715784223583463562856659899409874803093073672488018303144923030068676472051595092356526901054842204531097696259902932918208536564839131123267120274851983909949499435612911011950445762617074343617361172030023077403170241700938818654524089756160679689326380965420532371549776243873528359216364813539202381628407122984139703474079645179295766182114873749814059256198261702904054557021869783567098031115404925985893384236288158823312384306209956161743042018734597401478571640031164073299811264008721869825919143135464452349307665937910180388836191870940968213962457672875433367465699981342989921136961150851899098348962427247656276045628334498419725657337098975093141133484324045254894161201924068683620010789229259285440480995435940537667624338723921557053238834783359556037918809582458892065848815694769790908499268893985528537670155058564232806507871141747845307802141138296171596702078142972190344157573527052350264920919257149276054778817892260544282178683883575298950379106713711777305371755108244843767153525298492845149492539676125605361923275233116810043393465586954910995685714911037628021035007979630990942114018163565716858886952183499795964075550150695765807918478868389254216004025751162803939190400349139702748592191019465457940579442722336482514792688984860532792257196706248119975048849992057645749519295412896882442921216595967645965932548796390677044987760093638046581310350848285187187220706196305279160859422615301655890238371914719439512604956028472325461241661807133083764101212993222680363450924815481487152727734999850816912734787487634799653722673868803894636271073644245352807471545596836983252109901127883582221639923246916843189062194295402383784139352058193304060828934509418616514140725806356450819221116013159887902347671445693828064156122676678333858783888653733986439406151228238670834551021889612841163850023320599009868483797919409066530503649697321176896587833303674135620504707981450636223904342906387972709781465589135247485242411730955412829394395509150834636577318375653180405883863644913661431939534297039875234698815609730639701256989497969961106340204286279254338113428409830958658013062178595529444920940893371986893907407124968048619158409609577270430603466815630193209491312462978395464148307751819840613393872016461409146512350125062565527741676704252903719279146619611331982422057652123557378828348641230092372113188116938962185730522161539995109063603283676798080473012083470540925135796692002291135526084675421208266950756121264820067709438556300163995908298928326197261354767115956201929774895609476038381926737074585828849517708725081660039826488778652180865711652688594065324004418170046665840847385122149832130497696536519135414409814954630682003743374235166419418381748472225234228572015784725104136885769605326076472977452307785143285709857240319858918518227276606246626056800868716810798625325124898592420328697128551554705971074017976909339843310278671596655707039036654270573469107042977040314865542801532857044442026376030125511127711910520195685200389976464779307624182375947447509207490633197491632145016583575313897838318955647875226685098523620030364236084631103242292316144989828638132595956887347776151397749999451041179759955558371662375976355301795897889598346047816122213519278861117680241180567167592740997361610864011666641403223446662713331248965754499496066157376217004024133510911224230684692135698481617802048094364910115932043044537035298638436168290487991601659089084186045327230808114943310432416704127823057115374221323485849096087109098588025374394522470461785106124876322944196559141263667008637441717150315327015917909971218205585014246004602289124740717731072915781131702336088786596150734858463140250398613836123079551960480030212425968119770452253411375296256782692162124907041877721650354409568544199663788557957774279042090228681413422317507596505882532316223134127225470164068790338304677290310953674973337872117199105115145654589729684523335745479712999306818275435480079792503757434339282850753397875519167606827138455358413494577106690263155792160192153785587447651978050548581001207748589889427968027639404117832788138056165994824993638872993257861417428942478683501652700373945774195502016277949921693521738978510930512599695877235206703886522992293541156882883885845364923265105810326096846674316976857133487747286113219999988718237108127808892792150830241885980873426538338925828039451238620688847064310806656126849092765804586410553783507295081722133788761993300811431882535343000445429903153776387184344738021546641582454408017840330293010464342041161299831824813661247250960451567174326128927866031162421524016108370007554403025617820225271336897083007225614429445318623824433142992014763240554316112407316644141258969521056135384572539410934341222681235453345578442230293771002784659436569777948835643278561779978937348552487989073951618973208222574901496027661367088065757450063864060671206765133805689302520195433326301524947373271369191538784410315927932761951026798035308502054393393014616019239630277418978992096342018048979782769355374715156611880165158820419992741612702939119862048229714438875373080204014197360790786196695912073292011409556061943320214954811949909269022813090150735867286686781875337201962235790867902628302825116336633686319054774994267058228718582327206232351976439094652712767930644949542356458058685737078834464497805244819793989116959492777265624545236732965916140101438258436290806984291745481781070536932846687423825618096971095953941217880625557896885096187878139408586255861224781063217517627877614521116992898134727064523741788043765476395549440848927899276886930867378372311911094005051265846042837922959384642873002576853511983173209018444274808000891877302463159592054258466962686292512585530204452779884002567364440849018300943554084222654402287375512166652217241721150763808790534635356640067045452849217111181588057679374978663809699967048275354400511378072156148456433106583520077938569028522091792444337381890270030919886387803695935108682190185296038431007644395000377882426812371185748908772163136727343175204923656483914798530205944513306905297091319409134872153692699094876948713501080311027994830265454482436663505450339738292930266396537685892648444612432534057726206184990484084713520820635307364310326956214749760014589326186799843696415930926446555009374845103346959658112101662902597189783823662202462170188247038712253387049486825232648996967906824359711861097186466458100317638824369106495573191306241927984587903785132419171827060955515871139246545437581564636417977397324565301129045294541437891233719871885558080722155110694807805983568350561944057213454825585685726642166018061379261640776107108689641615973737732236264475400060525945443708442174236773553068755362725279187053289400441650450149688113498035859983828756721186080678635226889908095201191377400312322779410861101547806963846244873881431412387553398640531109160215796982710342457737945674040504958550270939978130913701447514571882797503605810615165445247528284388900178187250385980759157791475768004551098154175457280574516957138473423105722877180372730910682150847179428781166922102995692797597406048917848268676940043094692504694875498917631838274134126179536589213130132772262283992959535751591548787666484695517982143460199018504688348663463658632623508992763961548539575996037884094844212060925646970866737171988786707562004103428017169527815630364300330555973166157844244534661982218819654723203829657220131159031405758626915082720221702262905152679812247707718485875926497212030134089441956989326402330295338748626041845311630560310437576601788354726031715406823497387369025491281219218899805434284324225331614704403893761151053481120291636661566267907200129917179604203952890202152334963854533561581196407801167389974413064655140971343811246713771136989625466118875349226231670054084808608465161263983827752332751278411064020230676693084713005614796249219060219065345204615428189385506307450801837384094376240333163878047516879951760064100234666212099854550630367863255034091731370026295528960891125946800056565153180852980915477842787304143557345470023854294674145633338914075579193716119823250534948969164306271005971524356470841035076906230693831431631040949526977257959427211312132429519443936289774777795824418169778407039216829029590766285330242641425528415196261534511733336082433590737491911776569436878044968491907589638935463016896363152332621778398327558818189747486343588227098111620865099531287625683165641163329763988366160924084757102251895488243353625176420668675228712745314959475660790477987750751477878773945992773316871916495597062529775202472791812245257290471884388786235617541692496282829223226982112934963995206196278566855479126861792376214660331041656967510004280471683219105147013887750218191938868797076424712308690336917165778135793439081395159033760089858049252884882940452092941530434024082872911274003925542672023982285873168787255676707671517040638313291316306410480575571597956382775094137661499689708759307707993773285787265611376448686656879120283444193791937718564807283242587600789881914607085318623947107431628745036526175283900008293485624700848957536791768392410837383841119385868222705174584422890176278411213465516257047741156963572833547233735482772836734238602983041575602524868547028822032277677979230903174432346225168071511024819957165107231801829521569585390106328223829281093998847878436246534552476442253390775973316433973878162080272601964420861436718046743564686415390261483685964160941556412963174365098317001446595985810389594921569264113569293985549351131857716787110454325825757004248899448377956081468188999623716149296447376381331277654674881516766233420005041844944597039905685409229918630537355483953521283795065868999820592324520495023175735241346909526082377573052193683300062256792530326357433736725665600480398789148258241228150123399527561199170351812951551023433118954045759211436904850219253825761347105042795707312346469218544760469293184112700735890315034425510243057208530788262487501902029555758099661432370368046172619741926921136388940370314656687431481911530325715889869113068799573603845969389729445619085624696491926288272412467510524893820976884594157263977471396694779026342215269807079293987214205972786035647615449136908274525639266568559550572164266870903845400279498599157093265581423928698346537939613131096248080452957635186292445577502738869133649493798408731729026350042174627046433311101656758513243367884059700482736461227619290583070172992244519345040452491465187283144743324984462612082201685058821089200983011615635183168370046109924572073500804299655777841657063645054261088921348725197804276474044036688385562914241246735070947319617539503469850951717075670048311712514535796484241166479062890870710186024299928173948403509761150248795201952347245379227599295392147017715693521121150517476516272250773128157382973186523775522658254235343893672378615775736407064567295843026254843063627561304773586776492795804473925237658197679881781886551294916630969752028701996412663280277203202701461369186266390387793204888819826492021384944760895750644546739185270977737267413549851434328044770688001359969184491778395671489089986766505627202401992709629228668474194881276588359988732742199319773382884028554721806308248461546739697914709069253085806383841677199309327990566111895067798713415354442705172497065043542837774741600417115833703276230480911879971727332587059811071344613363831026454606637461415723621251526749289824248902574818543346444181203405443124615725950217300832038583105136443404187138303394938067427117250717160393639209326892836478755091184098660133270480110279864448183305691885636670167894382919634617437069236473638685090012670746882623628597519185633116373515486143971966319930008611122850871775097827306494244333814818488727855375404587799124917333185653164968429021443931644363404939154632911972700506491171705551644215908360154153662936719302135518053845590558767028771158649956779277777930539522116577929738990826171608153400886800891994598378813521928207309706640737045200476211686919331185653133381561215547061810941214015217325998391826590817693677350679594383114165553125144598401682427122526027549139007037862628283660689161531527737830500002231922830847291506377872964997078279591084568807684929897346529533607496869439317940232671023568735909460438220080340011982671527602946761598936142565590484062195451634129120286011429079828529225272835251978439471361748246086343680233942193663260152608884179480648854207901627299121345394258286294410912801208256618558873905521816167879238895393581728996653661593984532833875561546388641067429804730527940745879481281314921540530125078773798396809128327389695744737654977462822858650342020245277738155767897898339009528142543929050601802060313132622732510155959864788479049033621777423730186261106962760779378817821612829647619640824577119469174978249173973286608474654442486298095537653903796274435016493412119971225539987729901640275511769900724066718324882016387155576453141653416670916062797435983323342234384962515190767173328666990732604359171766544430983391753740777183948411539656040246016839810550203883607716870497230760221019017456071833423456768993141495335541591327374994689859745490164219409849891974683723446064487414986177698927151005586689794448053560854435622968854107628458450122051144626672332184562162823332898119508471811118258275223017910967270884212702184268452337911130360563761927639158087216896779149719757228700050993031710384781702560126403084066994150288435568359878855497238406614146307561918512965040535851046292030332316446870060520645685332288928959944937039736419560564492947887444372620974845468402788501254635260072274886561700460916216545118838994391260192333299455675288088201153091517335947538882051733428609304251128775994295431489162715563927622468931446136478133548751193172142026213263627881367766329545166346611812014036901579064895032638788454566713016802650198748345326410300876278764931520614061928900309894384417742288826734792163292453143312934236183207268528382758184072213214253228630723168794569033779996107718631698013788890785355978764935051839573703220196736505750151424662584315277312175240009967843249793038128997655275791816324702020197828166070637693038324289813648812277237744941273022037169645254278480839457538698472689719020386710240098799861563630479350993008458788735342276843347602708444243761166900782899528858100400536604348419485291998026987224520284619209752046761544992250954877354593407773466538656036327767703232613574859056643310550100477708072856363745568507277947789973476916727831417982774765627454120984447104261968830370940327622201837908653318035010423523442278921857549114804706912761629503496404146629776674924025313048353366777552015431468447481685565902361683216517598930586724285483835098345390323834569084532916207937098741357729741838220771861922232069020556336527672267763547034245820978985446726439547759953434311144372916800371958619007044499056696123305739590255930830635629131996444744093716262914938092172812698130442989689826755426033042112972214788740663196244324898146272142712449194660802106192473782521623372309747652688764485159752360773191236614046389853565381414041750004475155835980827373889722866276848559946847681850779785030191031794355539184230000830631166495826970082521456612257664759575498813456539763099074192748220358708609282367413646966204515486795759114383399458711470838555012281089525348031829609165120904225268512195139645803858288375470717684302269719788049521622064347698081579169059782511788961135828221889713090617073068933748007497704099611508536656456551676758397284788662689781000324609252160114630208101194235331416788402059471890013365796345844415664468855627019486983300824905669597818361648608857711076774287797492377247374820765933576741100354168275568587107647050389270121828501043944997426825423056530540337474327665713590899788491428763701587730858762375779918702419130615342388080180080801152228286312124208316472290377298241947375573755230514162578591401936855516630493672959178510776025825417266808621972528984748138346303854291173919334592769007045029172663057049441562007091433490788126740667430609036355113494281902199863289089782697692091586956943188243266313329494453676345126758351019108034879128182489131089132145800834698082273887819862105750803985153093278768238191450040408441802856737060816035935876292391595717545287557584702911894093647161799300458790704079245296097901563896740424122720840084474578742493392724818118683458700849285740496920970269713585215388277443858768119985485849213001337868740488663012660800323330593056257438027782933514638356470407378404524777323766038181109983865817139883268908133620793750333689789511458195380259807460209173938082781927241408923619969338377204749071894722457556485538481942439813663434465949738256052050058689535919809005679228906018858947385296570858218553478431851897376716283766755800565789600013438526248069249893279252291608314493929805070249154723599752255826864063408884823403334863058343005771412481889319305537695226840497195675769057714846238093122832496531075098410212846713904018275277984696797301026509792422201551567987031884374544916575821652618464299051015622393641911698470801788130023306600823183981164797872082623573119191740677607766756938643725707442375339885278755023322809068066933591242616654035935173346039875145235000832067786889722986538855897896833462848437851030275176860681182561136401871771890533161331826513405342105190943106951587036173080037417413065202864769925805524625423098805604934853478223919591759653051129796006924938682759646001409255205865864365149045595186890347294979889486643307922824699653731598084137090576410958119861319691504171181416660405847896089304583523835457025830976956968076138128485531254127541062116625611287946012059359714524370850126050694120358795006825546604991684224941917736658479545478784401676171096064773918471372470424903856116086025145963688602927485262356666347986560606175997262778921705168197562555805346021222095956228843072555141351791326400240957794370210305421624002126522894637588078560568903628253843114516716197081196773881462918365124759962905640375175188165420259862381036196688894127631473474576348475789273458863395949044316476334294548433572370296091515681312540154517908667023660822557245845556152877367751471325940818035022895229511535844057045382030321540543219768492036399395821692485883696843468683522964189863162806604008968650836599978983811359270788134106438898480525960548922202939699252434358889619066936811996720810158794691862259709840591707395288624518334221181632399411721941027438066544396879784810832351578663451794163974325137591493472826401055957904336949585053534404029516110782375465006381203794319782441058138632534976415467474068816083317940933300809976797036996203525909796926298722009636837030072751548393098087011747475501343193194596871919247691803933213137168104319767885419393785577293956470459907477882344916959095365357132336199941564458107223349576078069657965476317186388510011149427281086532195532951324309786354467785453758622355931004634792943761063617545629120210157135240734401787469974294702188465337829163929457351576679826804242318933953114790898696671596839328678775941959020185470214748047799599292177634903251831345411798294462831539097425136715178151128810950927198875966679210388386277016694021128446843798828614169217774865789786297600434702708658855106704882740485789749223318879256671625566131085697290052680718192248968079475623087787587865539597040585954086818841395630191682783510449029860243358697501371625757157506188205330145783006185513942273243226800915520427765703232422843414990244828170204032759714694573934740742792613809056815689323557260296044073291910833915375693690409618266504774929400250541148903311711535638925595076309813235290904302456509829411666133786684655169680875992040866059862295313297374269076308626183391432159379142382612741341944930045745242322280978383361828228562319683071922217396193056051296749439461685572584694281096426946879231569143204544979709664378725067222478276376540706414582921353568063768953749206236138202803532180481653382731911216727286809872929687588556329226607072543417856164563413586340526394275712465266133961344389019401608746399202445627669511908602098810162952928484222364033871086491816449824381887704750401790112416214777036542869130422974637828309629107703205126171099067900966041298623943793639682757264193768224313598564554648703917963066366007967424085535251090152145883059856922493342173691205872716758949009893032374913175307338336340272200776168262551102339043789498219215790922336323597315990326960227691898352988501719680320652137002630457621799622771621588032741407872481730595476169888897614817939260547735578451914194407723515663644581568990301279875163765908317658161014585193220256022293208060276087915025397072340429898308116726630472199385790492144604427724900915433569942167988667461917551185375207919725822143409730384881658664774899110887243608806159712741513468633105882819258049558695007382747977387231538093306055680998039337658546894776907577509202310006029781556748616590287527733614603540118631206554120073600266800599122165423033659633850657280764339227660011881745254687151235507920019678904276802298173843535581114015596084940725459796969118255658206425853401317473530368896859490461735903957323576766199674642009273813679232112378070612951253299958813666631515187724079635256372068282036393994308224272908437621375369211135543437271622135717958971963199200200174769819422416533428882801463198117209758274094506005040697968301354916616332766401363524419082689419104638479298933502416072732766475279050631992768355291942878568209358415122404433850468516797887845253356832460458716146237691944184474056997084178326050844301766115075480586459596930812528166545987899314559351149361032941281731263500981026780168161912634903084062804208803073624616298560110195188531007103018710339317674736447382230228824787968626727849773720648145109606788151328667262518944644150723373622464814482352906449601112910977560622957508956383371109805751804943274534156164734766048767210424287336393509907939167771253947812153412500652686076691184368370465281819862905160613623131755905846070170060324535308361648479268722736860032339877619889912811556918025211010758616060865074702879888761882727318826012309841958329747193281176860277846665383884842536090359064144099596571928997815652412895503599755826110292410788933154284397171839441030342876184912355731455700652418555517785476622733840128352208119829923236351421585284339189844695551120134050934109307351322194134443466467102022477647396438650148575961502564854614074138004775450657174149275415666365449744279139102655596807336658842194462197512391717597517585720518465153912759974239242244336290262996964659495576861459310983723883742581398619023770129130357082381413880320361556843573575662243899781594161667377048029396802492615498650664875917778908162220695122455166090822631497470087002116476079588983214236010153064256642006850858887202036364900888524031882615635606219528685640156871420355283865977193422747997876889779778076200113816359188965665753676546074801594538322321570659393640884930345426233854370320054624339824716278391660630421778109177178094550719795600395668332757588756528594183339554799944391729490661522407614545575352660509956770854314902241817499569123276371210892636241291754092236249079176446906939548417092642401362167610698094801157696662667097108309711994878085658168391767350007571574149651384966023141830601497587481635473735755231650207040252402336751370205965147095914134587978930593512117056066385636840000292332899079521139306260289392537221467935974532718954436673123632289185674002318453012177733090156396537634855217433087896729528653858662945623971148481038740225667482176213754609490021160007669442816410591545291311764237761745553949884803170458402051095250677958336182098728378596849921214223270615838414088408652175478174120231192163792679745952398665428212848070326426049701107179380504911245816582967802476576462979472584186831398207653350519209431022997484019102579098427098943384962738928456090231570081480555743539596474381967984126441075602082484573282858779113978650440304823274518072460642342721166483428947251223296963557921852880730026190727279979827930375717952552460492402975360122394096123591208169346429597481944369094300931356199590159408367567645810852738013198953805531255702674210118666710012634908538164036885507241057284369531554676103610257679568637807068410059437532379687204845182272306795490734605763490746362525844636822482191011125915646585606361401138259498450786677712144549167336736403727181207189659691962662147869316972264744001212760882541161960609999250064563686300277647735461932421335441170066181633214389113474426267886584764937840380649408987563196201924776271599251889123242125957438644500590346086398767745586219618071211602868755809226351783247838484993947692790969935437572465678982942194415018457329646015487922725464459150677492450865395336440084460647972823802453055497998726692084397905041877881982033081253609402885459429930003594486164278510549840681753963337179397510666575587176250041464093500772522963047733554437597808186449555569573107900667379716351671072971287190844236902160716459722069379657663591902427191487407777189243791923176100067053830921672511398340949121179279325168510474414504434360914570985332463824945763265020560134641222405757441919017832963371227143327373948433609549881244840206475047761099849330255507918759227438825548697430342383669252745896381948627640680460271363484546999112388719740923160018183375501582964498015370577778587634636995630420030073416007736791369258938879122588475707330779367277388399943982257345586967315833713231309063451285189898458704212863824860543146070094528319375805981239813086331126264950261778152392741511580489029866283437615418163495239362130572707351261335573423789787206382481220319148199835843469789263924152032439504471699520536887381734470955592543044351421116733969570315552127937682040075844209233436344310386298755742592307071556896662305367797767067307048905640405039215893785325752490968820018743179419871703806340899344887500583252225857404916509798542577689809779764172627450255648755864164831518955682293902959878639812454528238194447689480937060259724703701193667238990943812089340622210662717094772453074062103279984664555852382214653751476230229815406413493143844727934862161588277902571461739155553450745189136438646131567632416614632792321574229515199581678860184187476860999133547696033935356378192565405448169597993134694524341685624017594778243877983197731260029401895002082472344193204551432127114781169010950409396485185638478644673617476140213436012837420672990784904346972031485396566894962111794168706971475965937869897403732075851145277862108994782799098638079528587230895360846806129408600010322883330075187744428980090275532346484821329062494694312683614105750071342798419852988930880987185349259667947352590617292938968020046636311906560184160927036595104188984863356705453304245247519464370953946611722088022426960781465062031345010198272431567422341865837010140573431155067047061706154485528938194911974945888542127480105774053306337277766568762553831736348820257900004734627129394674487072845413614887668602695748268949991763539237935053348542531171367121948600168676442609434508254523641723890683345050968263977618656568991793308102748331341101559677597280848376212962102961422135305404446440364812220470204297831897617797424539696316668553521190333661331371766164410489367535084647959113472436962742696560689940598693055238471733248941787836650171882635584246677655489482302424778474228220097884577613932400313833934198243340018183554384648435670122984070963735519797688183725629919986356771883352945196762595386971255848219224252196990851155545959694075559171719280563601281151015238345517250239007388223923915151043372693899731588113263486897755140446583719840261233452106753426340404420648522951491121292544957226179254276797992084732256452265675736821410193849910338176271840716919014877527946960935980007026461716806336979702372996692500338861420434683968075510512136228535764605308843378403685351404719911097168424495475119102986812458588792628787050218897989141286292127545558098249257570296963209384477458124528407510917944913015338899988799098505602362707649489557761715110941575453328760070998146291293983229205436369725240788939415460206976046253110055273657694690338215844319080399172689422069660270477895933295735402279015983798907581847120332066308902751666297331163120375439098161496927212464763652177958737448443917342187068064784806262159101757191755049164151530528325147092884463547558619923683786492041054559412173809317208343378844392650085636301100828595898180005083004577457157751846262252862576145299266352847354682796360228907001456443280939849569683250375000520588438356169867200629946268208089107888628320390014764449086335826950919408232204726344322872253845803929870569442620765220371856768199087295345550342387395861685980578011232814269101936863958854143121179121570843702506451543578964637287934266663081378973323195483432415085477050129704051030745249925969571424128902369293776766919819319328968221167975137739893267438390727228689895512189670021544743292241597856873754685914255542130803667285010932227403545079243416723185215247634955755170436403343534756131264754347245115531297822841001305499386930907030651889704718238234394460435362196855802062601277068163744952524431910503204565162879023896500179324210439150452772202530094469886356626368045093245789543821009562053965899314347590823668167924599828384015406508502776150864535931878026272159437695652782249095335491292551769267482191990418624862251234085424614143938168198948796045927022946358996274144250978972663108688311994467802693250390531560120286830569476159624302151120632771899643169898446667011734540113237292298329806004774786680074636624683236357054484469183347814785626848427737855151037044667867993444100581490546283475146089502148497147382010318806126893870871591796537616935309193936171286576147136866387301854190005120269050637005914526452613643446341644626224132818104671162637449889024581899986724524180988173586688562231317389870971920791621392628324355857499246300898989950627918720435223349537656159523122251792581999042591609407851455603989840012214592869602640110301773600192604713279325685126564018467093434288975133906068851871122144008953698109965418880328417532072591180146097311899026023255190827692724896799425312318803385109276696921147976297112034211137564004908525840464057251184626408045762278079711481275464989989676838455020173213631846185960335324468967117590878588602303087267796890361064910880916122148802669315829000513583980348729813331073920237587969995840060484363468207681826264779761402643695519583481085705121406334112104366824965970338191570832525296476770887423356105280630762742163446190152072468539536901837191297281423953427683591710054002119184633086640243840538428087752480337398736975967378484899309888521601289134590188232076266018020550790691100222714301729415701634032987040887742974631450060213667235319863073298431742152628611596711800660235432821337816675911182288368011839464067337766519101815254401974753398446400474965410535283778205518108363361870813687975901697434485459538406324341851047048882706458118389511202002102862832231793167888704696748264043370815746518765757992236253595783282096471429659198441153476234190753678508297336042662974947582278420558883042833507157049270472146230030938331122000516272778372350048657652453457311869466929179442667136582417919268963907432646340217699782528459738614953234352415475391471850619837305438621243728402719385386531892348451887547831941573258987612771374762696691200508070035824317297836875432552268147826913072879712357309677250856844551763590023146999829493443641478464128392201862932153241386839588174828836107396631951907855998695197941571613196932406190185632485909974390191812599848364039345184332633637588739885671481469342909932491081773292427027582327983874613373013190138217232456167490810525953114914292618288850205776345590080379387630256887905535996069446140956817895952246188063511682857236095113288753230191813623761940265118989192656250906119157011525376178364396301301094023721463246190631473276842582544155977556122814233970906618648351438654774597335233167462748320830109069354160650341267659799939602548425936762628681401622518591038370022197441640854116037220601060694791084317215676710068053576549697715153429435448273478040627909673170490864512644475868893250194275713398210393151570891647311861433134743096952610417931896574144224851405473261699392646298230593005856608111752510428803509029222739996301913538042193407957678358335603559225814711751719593562965292620302143682309630005774506242238736541669312961549770318523913969510476495639539114572639105752602038946527492883645761155638480264155942081251440771023456041511893592806148082345589542166092591650811324186035135713162349664301608576258013876841175698642011851763671748698293881431187948707433751864193599571330270831233254384857855607001420421675692376588667490901873559069595323008612315556048535622652633475736564451647527594488484388447061288975119892104787697316659464794449327429992159009116811234829565562218229309468614985960789451681212863563575684061998316126803102926661454735933622361316289368938498077761884188227915436467022673040511831944741855587001144543642240423031042794318659685853476470636600039091038159561904543985050211349181873483560643705796283193167333735275804965205199465841720518231792236576005389799120015477665206673424368369780288153827516397592955375305331192472048272839353979678499956489051267131748736014039085809468249696967158507147750580248145045115926147621419596524631007157380801714170475277899367610233165376029273379643827293487332902728605700460638020323610679241036515851728059513275389915558383223578345280386382142666446254587487780174989674200840247117847031221675972884082649069005349800647340010524187637631286140225890433817803243328541770281178571889866886133611671629953666883224149611644224856020794122422924364774830115856190524537877325509702806992340505349569006585884333967798533696241771476535540669261740710968333559335353431529351781694087661252247917784025438896082062826390321121133984504375487998103068884395866604862121658781144896705228286159474266067836557554684882223853615935310246403791861309512239835109151824488457039274484214123695452636496854077369351283621885887360802099233857127227321664912006217166583532300926965742800766880296056409115479838136599932647411616727269491603605362285591873612378366901585577323771427521703137250084993098764189554759320415146922360337721171319319922508317071465134985967623014123900611511149010800918264380463368546435276840931861965605680226282278318251413774538525038362206518808977652212143273887202185151848902768737626850070015126270195338091881415286612615034854224529271385281577958027363567689834004697655626909389633304115425365888052883836801560348072809310913507794235481352173178226157982386207873061553677062628468771157223887621686950806239963873262325843302165477597784809945535245648563842758573560879382403211996950840381993871393936009187336622340051551715493752712034498302942265788539378694558333920055509732768366132669141357262829940052082416995001307651171523877010344886143011852893470832098073071150730265697915240380747895465322031106022338031813350505924718281468691794002457185142841138282930965561367520563595810611705110080616907264193688661083423593636608814086221278027552803573510773714036877207847906631749218041804805914454592622469942764801978453559731566560829983382254390032240305006864499343549248509791471050446751547277707930370667357449211090576804209359620370088231951596745742313606002626907237873196710492215955153164144522894394256551681178482496344193293075485526020092167145758940078357763929585819421748821274226436975133909212972559534548963953022730528728586136908744832269643108146608222425837939050374635715287456273075553530595801666323190813191028659570828687923543888890910151408360182851261589984106547882931328350479574077374032968629500640353529541480313830772391831430224877420664687994959384756838138046392390251041542943113333695394271143965882564914847803382348795770979890188966989193096585030128749517125438730777211307724168886369547269595831603406887397521272371735639216936366467181715316667186957725566747141818527674694737947497825860718394053419827818236698167364223240418887448604699835088242008936185931071757251108650897149082041046997072664155592397851683957566208247369029295861919702978163141403639657726470648126859545281098504283703514024900430462117455080534946543888262096367217780352157117756768143754122920266746945163423055400933561953458036042330087503912680933613529082115553504162586401916080784848574829026345515960728256922292266618333549868600593363064549808871491223444813445529546051294657619786912414598514524885235131162542687767134432053901120881053797276128429908515441942408396954363394880485299076894278619053994233613657644789365866107695426457134571255136322390961169840783782181422821338094998098818202656837407456629923677043416003523312271349800807176355937437871836919876940414481055077773016288250754010349967411012975866899680743745254757779758628198400119283930739397011958750563232830648747185122430118707103880490266301707216610129825769559657385251034714133657336623583098016865806510914479285686757352064469631738213856236793969884693093833094934180107511358608941255647240734425967620830540969978286403724752291801070702272932997411721845165276335975391304088030427511356493892792013272947128479049077892975491774590660386019046679169428700391159042215942861256098235757527593288554386153951218382631804936110548526239052553514966282294594552029582048806524062480948223537861894678289206093978301046360552603026968274186533629341922806792641337149712311442452152423194129991175394101566042876065083043947095062060245913327517194159340199416134419081515684974249293391695369827767925231612319030070663717725968766060170803839644677072076388347212456791487043969185047499475192813165272709942919588820707031757910990224431110835501029469892996812555373683624570918267768916108107506638887266300752810284621023860083296474354376136257532915726397305243701484841578897482939779728417713330174458335875244054376917086219149939382495380410658279009697251203978989138754641452871163366419306970205517012501103266556121162979450547475603512038998605241487328981888646930525114549303878049809540661782598265250645934302733504991992445417067441816410921994571822943443434111789877767187269754668995090746692286970062932103266497835463820791603100693541456383271068935622723533591625586778925482221528340744476849021879545901289621324543572627481752701266623167179115522867795754976392874263325057252556845800900334117574783031268907283521231134866913234571438538634820517473487729371329416466776045798143418607795132760280771830237059053177004202201374127965960331093015812473690354054959307768686124991316695193683072082459970462214463116366459692862982701174048589969476328000965688083007675023522731212336429956508077638529549477972165802319137380361327200258256653455051679153910662167228545452864440698193601922138514415170880614088733950412008765368952071862233351400142422591335070110429087440862701320979491233022328590772328599968428337017660250605763986747586004158923066124836817038370968755787845107454528511389012221310057929016648309904842530147588594912175269441149705953009538318947363978552777807079782648316896364782566744834199193027085204369913211823291840467713376920153474361841619045939587645502828840861108712233140169421122686420944027551621967564013624930403594681711493234681086013990348417037743028092939434196363217320028533590453576713396268908616275518822713885545517966773168050798996039560510656612536395950880066805002004644265720617873762986514235806994898496285236819101915154967802596791777962935797579713825469260481167445322200824746138942534690021186260810672812347660162771703989327758755554096052812883948507487983442574229820677465260008121813624397251065869172933979635735630131268510208690321616714640501458167874602794773980601238069354862123911621587869951569372346207141588427924331767541089930039173445657487182230831406624458409048784184304178876546604548550932311276557864916384965019810522250654685075627391144839552218176287337510843387992440194396278898721960326185650353376849270769184437457092139002132638367133753710506651892372194601283983089541421085699047539443355534464463812113429690203001559225210615950175823177411183413033487439039948637080236681894532169067424091626139945641322382372167266194843199584057497657516796021516683963580568018349720381779222565823372496649549168133980292915365999923391079685295152787515779205336447171034181434279355965560900092851620819006319069649888117113084875527808902073894893827316721259564664946293035766758346848394663125215312105737275392288161770847892621629847502889475033184493460896066163175803565117118590930599525321502528008483254538448362070362525747140271310070241152974382254131168703510883796417383767181124922545716647932146248847291472759038168425090280883366918949045421256381215632183213794138474032814090564817844739304594174472273616655948076909192484855525649018143465716250886836095552698869562948740927538364082458593004976311972820018573954389378590688436420540486791731674027351655961688654681046319751787423841105612925767304571473417266272658035013059345348847249042829311577170216519112120596535636100735036134286914975700785045522985839510516824501744710736417680113472567919084933378626991734988743247465450775161481972579411718728846119177955109672197270310886901691295917446995584867528277477900161308808374904418756485630495437772357043624368682153868959414582586477760508435389597775515759300510824578983021154439987263876103282718008344284536080015615662659551967995613882611967167839584056305540634466254757928617733543944677494700631083505910886915536750462950284259258463025124727523900807189518476607660482084002411803153448446270626078597578018306789911666185721421447405582112569396905308557158867066622170357308243172607913226979317141800594717558898117444312565319424573946931286138888277113874011273544430543624155706527673916592109700588961920345583055294615698566955082148009854544351561641696197604794695138035445812276767839125964911369884070148095249609277592622470988586912224573111173187139505044581111654953170111666415542771183122762403184011536166103836180915612919763463935269491277734790928113918350504629635434590046563818015041912482904852841044640853454844311821966051265958356641705429202170251456648916509540675493872669120253937349437881151591994418909530624559410035407703238117841494291098184021023234875377987086722666290165582285049341690951078436498785498088331265396584728692099874048485683511299198993324989654543507294069583620742703603881484153846787795304097999104398218995230022889022108370244053526740929381701281749769165415356690432480827200792952944825235802936782513467653005407503247176164381363318050058869019753777654761996156782118269779146516490957471959393535176541247221735331877180963401889343991585728029547130047731607950869831270738709769719064413730477109187822141426825725578628829254527980076497558966094682693241399460365043523072128608671833211517705275254104375770360663038024323072656230647371476897209067378923405378402604422581145034429897618066259481938369415558510880218502762648756107862540965603041626649236241372185300880567433980551721314813799770741294327159890025434635477024723210143454030999954785198315240622771740131941163469688942349186786034881925317862026199907291093155766385219356766165887665886538117852476351751678888414527457930408308933709079600550413324996711743340671222325092305199999289848384611149323138129862641553344687140657184143864032404775101499782269401463651803088651569735982086891846644007351147460051136775424668067799252277855973233873044953952511601761330908617004285253448608213845004655578035027746341538356144811072661197648062424208339175051552614403137777314768098286545010546160684754660055545925658907823006206931757457124600690448293394245043502638698969004719402265416251522231335387302712612604475598298987893947826750562446769596133062528415892961536114170541377213605869807368608622421841722995864499456299828794853108748948438257705990051650984853198815375231989795315434156182332532809305414604533844888791977415504780520932434877520416521693500379615048981613559924096697082585447331106813876542047486631265676089945500286758882533024361817476238580026807632889846772992694489458899220012088580032962511392419476124595027517727270920877332471191425034387469454961219050868153505083112037273582548841576964814294579417116056515573009911547769795815516565902110892557581103798785357528645712483730441916370530502543469057141234246580904274010464460406902347013211749525289652518959930752742673112850923471391098047835846900982809143935488167707998727796603892045076914810223753814778858611250837481120969104500240607753181816412379481909920295363977591459380871118983704374737539593832582636964999559086393390755501635745756731982261454824469966879452529759521009322949849671703804618600866238299995839139549379820007971049265412013964508995889710518226738303933455538959490819754052577713053745700119561220671085151262903489020506359844104476714085394026966254103660060809895750137020388897008553186131047033077809864314536193839746034035177979695772682454510490566195281347294497005351948689927652379553677269421816204671772934153770572516833409734765659982974863870406255592426987660499512182340749454320702451285187955606069798212610390742294885711508179695223404200994688733893178973240128748287982259078848003055023415442536425111220415177539568516449222473511765430060905327616855380557081381752708159249226535949689875334269167889100486019952792384580138728043047711872093349961716486665525168226858139560926048875982876879688097152489885537635073124941830098233910279707456931324527060429453076838965625658498238035926400773473702241356733985261473691644600332571397789825452801774762746844456325442628069217306263584857230606588222688155277706020925872132915330533298678561591571928107024611070437216962366925841132410293290399125589986084149686189387490294391564896315194745049503386600708171282682059689943310875798358212515667100131019789314125099271656081334469355212243389925704357362394861070109619067861374309792242994018498271253012132307950348478243309974024246957367322483235534244859128954308917465935369819850745438267140377217699019988171013020143029151990692159493068293754537960667122421953109359777072154414482546529859399057711365114124350501547864103153740878938344419086260402211776367557943228722191602207442263957173701551995038666027911682234911413138229788246525043397048748324529980733541499702273597369948406627064707125573140141992866112824476746036275924186299624723003194652532756050883430695151803263817252046547294947348808319088360706469808783699075856744668872123253192024112013604141105202819780073142323821273860585380526713428703221697356618998616947434023198909839754518910632639781322110800798576572196593380186598090407730249921068359150819234157416882076248012687827329731279335081372449197259532273942468762115622173668603720064418640678838206434844085733848608431013574898195666775754821626279696496622351354648756495676690230784379262032263371263941092659529789668842205052087441274813280200371490899674552310952008479604223414945667406026022496230127071586038763097490739947864656344495405142366721301237952266145440992667109918314280390383492924287175766479111769180685555646705333691126465649523442045380371840308351673765422623703426391608373892847543668231476273474557037361728977521651107547109311093170585259587939523587588038275924140414414510163582880194852761549254624509091832175723446671959801893351222647852613614022031758127651632544721323594983252870163537359772526724253260614322475537530641103027727940574197286800410146792340019323264078143861647018214586216025411295191874356013009660737554252049292385509918147805521247392505388876812754635180847641663202357887111443907112207207153083015622833138681790663329722806900316916671558338916004262753834401294239328997036036494531952337837226225870635027950533499171130488526373104993644947887535559610581098659817370424174857560630146538791790663471564031594434440325868711437749265035435130878827000455891306537725572634033368405268036739030321529006026155764467290628540212943146107908682962802560754418555970714618201877235643323593928967812115162682987186925437363066931146408682847712767825585437286577465691711061384028393788018555184147308726684061809721005056632779776826723521381450281960498810955223648107702424280176918012423924582013109161107305693507807176868330600743734675589847695609874453109095526360134413302957491123992385949400411013668965187166720437778291000518426088686148810562841852200548708101949918091747177395658214952417532741245685300496255736538093917745868705098761643236159111608781473043842781105478379004414203290881554893129863456007088840199082626281922409393287576818559493139352904275372370696758181602622583701731685611308197137919791853668503385855914207553468789071782137552160422817740150705710960521638024596130354513899406053174504054631681373958976187306750461794403381641334209043442234249732459332879961246478405587294682050164898647587557841432307981586378408274211980574578265294426126220612199621037455112788632122596987787527716548556740917390689495866047242574809583468277478805002681185629140622097109232811528300138615192109764242579235220730022029743274348593661099299050149270749003978563310852943301686059587334183933448523262853540019111620385095294148036961118879568408030137614173939320046337623976715576091711251266807940942508791865713761972189350782533758141799142351698193387140251798164848030302650584291797511818330619940905500883719674980583666108192303935409077747135163995119485023934743542368778157781760506382907720119689853222614037810042187291637749723931091464003176995247230123622112687735348615361724417686026782669901840144541499817188544366788766245894844441199264599281014890791119730432624720980374360098997971072966571781263569157390428971978824668972345585132453259130107578188533726937001458175464589865287225792230492350160529228884751483666094620724468629478372527829361735073144923189099140357496031439625658909345571162931576773513660706113299536410172953867428783305342359676064599957735608816841999456140929332259398949993759873073775479742081609001540625919041966692583344916414053933716859865743309342528091654976515730511453181894202916641883675517957921511832703774844826717456926738639410941207192926410936876931867044247897527186560738079251535497126347584989670342864767291873727615339329457526956567963984238762951736156208849800091006761534626444671776191311310020858339423022090102721189138116622676324515034597857933589507694817876553161266045680526001976730327447814725311391574024946040993415102624060534378577092704529369447667975807617791297805462083215575103469821546100271663244226786418336290625419523383473256966411330110038187301562634500926467268631233337676068146102383732923173715309842406489093766078293280420990202453831724408493581849743583851516606928637082924703093081992626520250627546802468346896126356292550995122813442240786240764480797759091672042216821439331566035456226619888682547259193511384360071872603258325868029332262708483973996045289152961248455165843613515710757092408292524380204275310350422564130630883778489014183051042696962123684256486745004908586044419626887020521686016164614079167585436302210113226159371698466804250664650871237597437855571460835133826839673382489375888306967339682497142598265844271807359667396421860273038058495350770052109913366927127821819137917093180959854635976618589400746684255370102196084981982883417860613837087614632831749941801081439160693324948091006855096976942885460329454490851364904796503195686427216521452619758539947832849583158681795639286679553687812926655237552179842645794339965102405387795441148336951405055620219299434242674684295698120864842326751578731533298565877703912440785507967810642537893420467506200963163931616995114669904787903603801526089115948708298191578882544754669896633876433124488934277795508443420573251080901985159841686899580221381172518184274460486417138845175617924637827947779428416261389335408683556921545088501854066778323587989559188663746661505736195428093734541526665124935586045215106383845489631962428445293813282811662075371918500520476100336190788225561230606127331024378158669805903214134788288127922622684680273654944539709067364738325833409510650603125807299134783268862306502630844152385474906330735502386196138035052686853408261533869201348521399555675310084221322598051406161464509956898720669859470877888037831605155671152065437510000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000, Dec Digits = 56709

d1 : 314159265358979323849480997010239689209316400000000.000000

d2 : 1123581321345589182464.000000

n9 : Hex Value = 0x3ce8d2bac65bde0000, Hex Digits = 18; Dec Value = 1123581321345589182464, Dec Digits = 22
#endif




// --------------------
static void simpleDemoBigInt (std::ostream& o_stream)
{

    SET_START_TEST_NAME(o_stream);

#define SHOW_BIG_INT(x) o_stream << #x << " : Hex Value = " << x.toStr0xHex() << ", Hex Digits = " << x.getActualHexDigits() << "; Dec Value = " << x.toStrDec() << ", Dec Digits = " << x.getActualDecDigits() << std::endl << std::endl
#define SHOW_DOUBLE(x)  o_stream << #x << " : " << std::fixed << x << std::endl << std::endl

    BigInt::Rossi n1 ("314159265358979323846264338327950288419716939937510", BigInt::DEC_DIGIT);
    SHOW_BIG_INT(n1);

    BigInt::Rossi n2 ("224f3e07282886cce82404b6f8", BigInt::HEX_DIGIT);
    SHOW_BIG_INT(n2);

    BigInt::Rossi n3 (1123);
    SHOW_BIG_INT(n3);

    BigInt::Rossi n4 = n1 + n2;
    SHOW_BIG_INT(n4);

    BigInt::Rossi n5 = n1 * n3;
    SHOW_BIG_INT(n5);

    BigInt::Rossi n6 = n2 / n3;
    SHOW_BIG_INT(n6);

    BigInt::Rossi n7 = n1.sqrt();
    SHOW_BIG_INT(n7);

    BigInt::Rossi n8 = BigInt::Rossi::pow (n1, n3);
    SHOW_BIG_INT(n8);

    long double d1 = n1.toDouble();
    SHOW_DOUBLE(d1);

    long double d2 = 1123581321345589144233.0;
    SHOW_DOUBLE(d2);

    BigInt::Rossi n9 = BigInt::Rossi::fromDouble(d2);
    SHOW_BIG_INT(n9);

#undef SHOW_BIG_INT
#undef SHOW_DOUBLE

    // --------------------------
    SET_FINISH_TEST_NAME(o_stream);

    o_stream << std::endl;
    o_stream << std::endl;
    o_stream << std::endl;
    o_stream << std::endl;

}


// --------------------
static void demoBigInt (std::ostream& o_stream, const std::string& i_exeFileName)
{
    std::ostringstream oss;

    SET_START_TEST_NAME(oss);

    BigInt::Rossi arg1 (100);
    BigInt::Rossi arg2 ("123456789", BigInt::DEC_DIGIT);
    BigInt::Rossi arg3 ("123456789ABCDEF0FEDCBA321", BigInt::HEX_DIGIT);

    BigInt::Rossi result;

    // --------------------------------------------
    result = arg1 + arg2;
    oss << "[Hex] " << arg1 << " + " << arg2 << " = " << result << std::endl;
    oss << "[Dec] " << arg1.toStrDec() << " + " << arg2.toStrDec() << " = " << result.toStrDec() << std::endl;
    oss << std::endl;

    result = arg1 + arg3;
    oss << "[Hex] " << arg1 << " + " << arg3 << " = " << result << std::endl;
    oss << "[Dec] " << arg1.toStrDec() << " + " << arg3.toStrDec() << " = " << result.toStrDec() << std::endl;
    oss << std::endl;

    result = arg2 + arg3;
    oss << "[Hex] " << arg2 << " + " << arg3 << " = " << result << std::endl;
    oss << "[Dec] " << arg2.toStrDec() << " + " << arg3.toStrDec() << " = " << result.toStrDec() << std::endl;
    oss << std::endl;


    // --------------------------------------------
    result = arg2 / arg1;
    oss << "[Hex] " << arg2 << " / " << arg1 << " = " << result << std::endl;
    oss << "[Dec] " << arg2.toStrDec() << " / " << arg1.toStrDec() << " = " << result.toStrDec() << std::endl;
    oss << std::endl;

    result = arg3 / arg1;
    oss << "[Hex] " << arg3 << " / " << arg1 << " = " << result << std::endl;
    oss << "[Dec] " << arg3.toStrDec() << " / " << arg1.toStrDec() << " = " << result.toStrDec() << std::endl;
    oss << std::endl;

    result = arg3 / arg2;
    oss << "[Hex] " << arg3 << " / " << arg2 << " = " << result << std::endl;
    oss << "[Dec] " << arg3.toStrDec() << " / " << arg2.toStrDec() << " = " << result.toStrDec() << std::endl;
    oss << std::endl;


    // --------------------------------------------
    result = arg1 * arg2;
    oss << "[Hex] " << arg1 << " * " << arg2 << " = " << result << std::endl;
    oss << "[Dec] " << arg1.toStrDec() << " * " << arg2.toStrDec() << " = " << result.toStrDec() << std::endl;
    oss << std::endl;

    result = arg1 * arg3;
    oss << "[Hex] " << arg1 << " * " << arg3 << " = " << result << std::endl;
    oss << "[Dec] " << arg1.toStrDec() << " * " << arg3.toStrDec() << " = " << result.toStrDec() << std::endl;
    oss << std::endl;

    result = arg2 * arg3;
    oss << "[Hex] " << arg2 << " * " << arg3 << " = " << result << std::endl;
    oss << "[Dec] " << arg2.toStrDec() << " * " << arg3.toStrDec() << " = " << result.toStrDec() << std::endl;
    oss << std::endl;

    // --------------------------------------------
    result = arg1.sqrt();
    oss << "[Hex] " << arg1 << ".sqrt() = " << result << std::endl;
    oss << "[Dec] " << arg1.toStrDec() << ".sqrt() = " << result.toStrDec() << std::endl;
    oss << std::endl;

    result = arg2.sqrt();
    oss << "[Hex] " << arg2 << ".sqrt() = " << result << std::endl;
    oss << "[Dec] " << arg2.toStrDec() << ".sqrt() = " << result.toStrDec() << std::endl;
    oss << std::endl;

    result = arg3.sqrt();
    oss << "[Hex] " << arg3 << ".sqrt() = " << result << std::endl;
    oss << "[Dec] " << arg3.toStrDec() << ".sqrt() = " << result.toStrDec() << std::endl;
    oss << std::endl;


    // --------------------------
    SET_FINISH_TEST_NAME(oss);

    oss << std::endl;
    oss << std::endl;
    oss << std::endl;
    BigInt::Run::showHelp(oss, i_exeFileName);

    o_stream << oss.str() << std::endl;
}


// --------------------
static void showIllegalCommandLineBigInt (std::ostream& o_stream, int argc, char** argv)
{
    std::ostringstream oss;

    oss << ""

        << std::endl
        << "\t"
        << "===> ILLEGAL INPUT <==="
        << std::endl
        << std::endl;

    BigInt::Run::showCommandLine (oss, argc, argv);
    BigInt::Run::showHelp(oss, argv[0]);

    o_stream << oss.str() << std::endl;
}


// ===================
int main (int argc, char** argv)
{
    BigInt::Run::showTime (std::cout, "START");
    BigInt::Run::showCommandLine (std::cout, argc, argv);

    std::string flag;

    if (argc == 1)
    {
        flag = BigInt::s_strHelp;
    }
    else
    {
        flag = argv[1];
    }


    if ((argc <= 2) && (flag == BigInt::s_strHelp))
    {
        BigInt::Run::showHelp(std::cout, argv[0]);
        BigInt::Run::showTime (std::cout, "FINISH showHelp()");
        return 0;
    }

    if ((argc == 2) && (flag == BigInt::s_strDemo))
    {
        simpleDemoBigInt(std::cout);
        demoBigInt(std::cout, argv[0]);
        BigInt::Run::showTime (std::cout, "FINISH demoBigInt()");
        return 0;
    }

    if (
        ((argc == 2) && (flag == BigInt::s_strTest)) 
        || 
        (argc == 4)
        )
    {
        const int result = BigInt::Run::mainBigInt(argc, argv);
        BigInt::Run::showTime (std::cout, "FINISH testBigInt()");
        return result;
    }

    if ((argc == 2) && (flag == BigInt::s_strTime)) 
    {
        BigInt::Run::mainBigInt(argc, argv);
        BigInt::Run::showTime (std::cout, "FINISH performanceTestBigInt()");
        return static_cast<int>(BigInt::Test::s_returnValue);
    }



    showIllegalCommandLineBigInt (std::cout, argc, argv);
    BigInt::Run::showTime (std::cout, "FINISH showIllegalCommandLineBigInt()");
    return 0;
}
