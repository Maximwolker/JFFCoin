#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-jff");
    BOOST_CHECK(GetBoolArg("-jff"));
    BOOST_CHECK(GetBoolArg("-jff", false));
    BOOST_CHECK(GetBoolArg("-jff", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-jffo"));
    BOOST_CHECK(!GetBoolArg("-jffo", false));
    BOOST_CHECK(GetBoolArg("-jffo", true));

    ResetArgs("-jff=0");
    BOOST_CHECK(!GetBoolArg("-jff"));
    BOOST_CHECK(!GetBoolArg("-jff", false));
    BOOST_CHECK(!GetBoolArg("-jff", true));

    ResetArgs("-jff=1");
    BOOST_CHECK(GetBoolArg("-jff"));
    BOOST_CHECK(GetBoolArg("-jff", false));
    BOOST_CHECK(GetBoolArg("-jff", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-nojff");
    BOOST_CHECK(!GetBoolArg("-jff"));
    BOOST_CHECK(!GetBoolArg("-jff", false));
    BOOST_CHECK(!GetBoolArg("-jff", true));

    ResetArgs("-nojff=1");
    BOOST_CHECK(!GetBoolArg("-jff"));
    BOOST_CHECK(!GetBoolArg("-jff", false));
    BOOST_CHECK(!GetBoolArg("-jff", true));

    ResetArgs("-jff -nojff");  // -jff should win
    BOOST_CHECK(GetBoolArg("-jff"));
    BOOST_CHECK(GetBoolArg("-jff", false));
    BOOST_CHECK(GetBoolArg("-jff", true));

    ResetArgs("-jff=1 -nojff=1");  // -jff should win
    BOOST_CHECK(GetBoolArg("-jff"));
    BOOST_CHECK(GetBoolArg("-jff", false));
    BOOST_CHECK(GetBoolArg("-jff", true));

    ResetArgs("-jff=0 -nojff=0");  // -jff should win
    BOOST_CHECK(!GetBoolArg("-jff"));
    BOOST_CHECK(!GetBoolArg("-jff", false));
    BOOST_CHECK(!GetBoolArg("-jff", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--jff=1");
    BOOST_CHECK(GetBoolArg("-jff"));
    BOOST_CHECK(GetBoolArg("-jff", false));
    BOOST_CHECK(GetBoolArg("-jff", true));

    ResetArgs("--nojff=1");
    BOOST_CHECK(!GetBoolArg("-jff"));
    BOOST_CHECK(!GetBoolArg("-jff", false));
    BOOST_CHECK(!GetBoolArg("-jff", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-jff", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-jff", "eleven"), "eleven");

    ResetArgs("-jff -bar");
    BOOST_CHECK_EQUAL(GetArg("-jff", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-jff", "eleven"), "");

    ResetArgs("-jff=");
    BOOST_CHECK_EQUAL(GetArg("-jff", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-jff", "eleven"), "");

    ResetArgs("-jff=11");
    BOOST_CHECK_EQUAL(GetArg("-jff", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-jff", "eleven"), "11");

    ResetArgs("-jff=eleven");
    BOOST_CHECK_EQUAL(GetArg("-jff", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-jff", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-jff", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-jff", 0), 0);

    ResetArgs("-jff -bar");
    BOOST_CHECK_EQUAL(GetArg("-jff", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-jff=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-jff", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-jff=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-jff", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--jff");
    BOOST_CHECK_EQUAL(GetBoolArg("-jff"), true);

    ResetArgs("--jff=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-jff", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-nojff");
    BOOST_CHECK(!GetBoolArg("-jff"));
    BOOST_CHECK(!GetBoolArg("-jff", true));
    BOOST_CHECK(!GetBoolArg("-jff", false));

    ResetArgs("-nojff=1");
    BOOST_CHECK(!GetBoolArg("-jff"));
    BOOST_CHECK(!GetBoolArg("-jff", true));
    BOOST_CHECK(!GetBoolArg("-jff", false));

    ResetArgs("-nojff=0");
    BOOST_CHECK(GetBoolArg("-jff"));
    BOOST_CHECK(GetBoolArg("-jff", true));
    BOOST_CHECK(GetBoolArg("-jff", false));

    ResetArgs("-jff --nojff");
    BOOST_CHECK(GetBoolArg("-jff"));

    ResetArgs("-nojff -jff"); // jff always wins:
    BOOST_CHECK(GetBoolArg("-jff"));
}

BOOST_AUTO_TEST_SUITE_END()
