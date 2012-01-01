/* vim: set sw=4 sts=4 et foldmethod=syntax : */

/*
 * Copyright (c) 2010, 2011 Danny van Dyk
 *
 * This file is part of the EOS project. EOS is free software;
 * you can redistribute it and/or modify it under the terms of the GNU General
 * Public License version 2, as published by the Free Software Foundation.
 *
 * EOS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <test/test.hh>
#include <eos/utils/log.hh>

#include <cstdlib>
#include <iostream>
#include <list>
#include <sstream>

namespace test
{
    // Use a singleton to avoid the static initialization fiasco
    // Use a small, local singleton in order to avoid linking against libeosutils
    struct TestCasesHolder
    {
        std::list<const TestCase *> test_cases;

        TestCasesHolder()
        {
        }

        ~TestCasesHolder()
        {
        }

        static TestCasesHolder * instance()
        {
            static TestCasesHolder result;

            return &result;
        }
    };

    TestCase::TestCase(const std::string & name) :
        _name(name)
    {
        TestCasesHolder::instance()->test_cases.push_back(this);
    }

    TestCase::~TestCase()
    {
    }

    std::string
    TestCase::name() const
    {
        return _name;
    }

    TestCaseFailedException::TestCaseFailedException(int line, const std::string & file, const std::string & reason) :
        _line(line),
        _file(file),
        _reason(reason)
    {
    }

    const std::string &
    TestCaseFailedException::reason() const
    {
        return _reason;
    }

    std::string
    TestCaseFailedException::where() const
    {
        std::stringstream ss;
        ss << _line;

        return _file + ":" + ss.str();
    }
}

int main(int, char ** argv)
{
    int result(EXIT_SUCCESS);

    // Extract the program name from argv[0]
    std::string program_name(argv[0]);
    std::string::size_type pos = program_name.rfind('/');
    if (std::string::npos != pos)
        program_name.erase(0, pos + 1);

    eos::Log::instance()->set_program_name(program_name);
    eos::Log::instance()->set_log_level(eos::ll_debug);

    for (std::list<const test::TestCase *>::const_iterator i(test::TestCasesHolder::instance()->test_cases.begin()),
            i_end(test::TestCasesHolder::instance()->test_cases.end()) ; i != i_end ; ++i)
    {
        std::cout << "Running test case '" << (*i)->name() << "'" << std::endl;

        try
        {
            (*i)->run();
        }
        catch (test::TestCaseFailedException & e)
        {
            std::cout << "Test case failed: " << std::endl << e.where() << ":" << e.reason() << std::endl;
            result = EXIT_FAILURE;

            continue;
        }
        catch (std::exception & e)
        {
            std::cout << "Test case threw exception: " << e.what() << std::endl;
            result = EXIT_FAILURE;

            continue;
        }
    }

    return result;
}
