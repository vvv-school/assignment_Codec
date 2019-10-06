/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Ugo Pattacini <ugo.pattacini@iit.it>
 * CopyPolicy: Released under the terms of the GNU GPL v3.0.
*/

#include <string>
#include <mutex>
#include <condition_variable>
#include <chrono>

#include <yarp/robottestingframework/TestCase.h>
#include <robottestingframework/dll/Plugin.h>
#include <robottestingframework/TestAssert.h>

#include <yarp/os/all.h>
#include <yarp/dev/all.h>
#include <yarp/sig/all.h>
#include <yarp/math/Math.h>

using namespace std;
using namespace robottestingframework;
using namespace yarp::os;
using namespace yarp::dev;
using namespace yarp::sig;
using namespace yarp::math;

#define TEST_MESSAGE          "hello"
#define TEST_MESSAGE_CODED    "(%,,/"

class BottleReader : public BufferedPort<Bottle> {
    std::string msg;
    std::mutex mtx_semaphore;
    std::condition_variable cv_semaphore;

public:
    BottleReader(): msg("timeout") { }

  virtual void onRead(Bottle &bt) {
        if(bt.size() && bt.get(0).isString())
            msg = bt.get(0).asString();
        else
            msg = "wrong data";
        cv_sempahore.notify_all();
    }

    const std::string getMessage() {
        std::unique_lock<std::mutex> lck(mtx_semaphore);
        cv_sempahore.wait_for(lck, chrono::seconds(5));
        return msg;
    }
};

/**********************************************************************/
class TestAssignmentCodec : public yarp::robottestingframework::TestCase
{
private:
        BufferedPort<Bottle> portOut;
        BottleReader portIn;
        RpcClient rpc;
public:
    /******************************************************************/
    TestAssignmentCodec() :
        yarp::robottestingframework::TestCase("TestAssignmentCodec") { }

    /******************************************************************/
    virtual ~TestAssignmentCodec() {
    }

    /******************************************************************/
    virtual bool setup(yarp::os::Property& property) {

        ROBOTTESTINGFRAMEWORK_ASSERT_ERROR_IF_FALSE(portOut.open("/TestAssignmentCodec/o"), "Cannot open the output port");
        ROBOTTESTINGFRAMEWORK_ASSERT_ERROR_IF_FALSE(portIn.open("/TestAssignmentCodec/i"), "Cannot open the output port");
        ROBOTTESTINGFRAMEWORK_ASSERT_ERROR_IF_FALSE(rpc.open("/TestAssignmentCodec/rpc"), "Cannot open the output port");

        ROBOTTESTINGFRAMEWORK_ASSERT_ERROR_IF_FALSE(NetworkBase::connect("/coder/Codec/out", portIn.getName()),
                                  "Cannot connect to /coder/Codec/out");
        ROBOTTESTINGFRAMEWORK_ASSERT_ERROR_IF_FALSE(NetworkBase::connect(portOut.getName(), "/coder/Codec/in"),
                                  "Cannot connect to /coder/Codec/in");
        ROBOTTESTINGFRAMEWORK_ASSERT_ERROR_IF_FALSE(NetworkBase::connect(rpc.getName(), "/coder/Codec/rpc"),
                                  "Cannot connect to /coder/Codec/rpc");

        portIn.useCallback();
        return true;
    }

    /******************************************************************/
    virtual void tearDown() {
        ROBOTTESTINGFRAMEWORK_TEST_REPORT("Tearing down TestAssignmentCodec");
        NetworkBase::disconnect("/coder/Codec/in", portIn.getName());
        NetworkBase::disconnect(portOut.getName(), "/coder/Codec/out");
        NetworkBase::disconnect(rpc.getName(), "/coder/Codec/rpc");
        portIn.interrupt();
        portIn.close();
        portOut.close();
        rpc.close();
    }

    /******************************************************************/
    virtual void run()
    {
        Bottle cmd, reply;

        ROBOTTESTINGFRAMEWORK_TEST_REPORT("Setting coder mode");
        // setting decoder mode
        cmd.clear();
        reply.clear();
        cmd.addString("set_mode");
        cmd.addString("coder");
        rpc.write(cmd, reply);
        ROBOTTESTINGFRAMEWORK_ASSERT_FAIL_IF_FALSE(reply.toString()=="[ok]", "setting decoder mode using set_mode()");

        ROBOTTESTINGFRAMEWORK_TEST_REPORT("Testing coded");
        // setting codec
        cmd.clear();
        reply.clear();
        cmd.addString("set_codec");
        cmd.addInt(64);
        rpc.write(cmd, reply);
        ROBOTTESTINGFRAMEWORK_TEST_CHECK(reply.toString()=="[ok]", "setting codec using set_codec()");
        // checking message
        ROBOTTESTINGFRAMEWORK_TEST_REPORT(Asserter::format("Writing test message %s", TEST_MESSAGE));
        Bottle& msg = portOut.prepare();
        msg.clear();
        msg.addString(TEST_MESSAGE);
        portOut.write(true);
        ROBOTTESTINGFRAMEWORK_TEST_REPORT("Reading decoded test message");
        string str = portIn.getMessage();
        ROBOTTESTINGFRAMEWORK_TEST_CHECK(str == string(TEST_MESSAGE_CODED),
                       Asserter::format("encoded message (%s == %s)", str.c_str(), TEST_MESSAGE_CODED) );

        ROBOTTESTINGFRAMEWORK_TEST_REPORT("Setting decoder mode");
        // setting decoder mode
        cmd.clear();
        reply.clear();
        cmd.addString("set_mode");
        cmd.addString("decoder");
        rpc.write(cmd, reply);
        ROBOTTESTINGFRAMEWORK_ASSERT_FAIL_IF_FALSE(reply.toString()=="[ok]", "setting decoder mode using set_mode()");
        // checking message
        ROBOTTESTINGFRAMEWORK_TEST_REPORT(Asserter::format("Writing test message %s", TEST_MESSAGE_CODED));
        msg = portOut.prepare();
        msg.clear();
        msg.addString(TEST_MESSAGE_CODED);
        portOut.write(true);
        ROBOTTESTINGFRAMEWORK_TEST_REPORT("Reading coded test message");
        str = portIn.getMessage();
        ROBOTTESTINGFRAMEWORK_TEST_CHECK(str == string(TEST_MESSAGE),
                       Asserter::format("encoded message (%s == %s)", str.c_str(), TEST_MESSAGE) );


    }

    /******************************************************************/
    virtual void interrupt() {
        portIn.interrupt();
    }
};

ROBOTTESTINGFRAMEWORK_PREPARE_PLUGIN(TestAssignmentCodec)
