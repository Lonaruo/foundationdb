/*
 * UnitTest.h
 *
 * This source file is part of the FoundationDB open source project
 *
 * Copyright 2013-2018 Apple Inc. and the FoundationDB project authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FLOW_UNITTEST_H
#define FLOW_UNITTEST_H
#pragma once

/*
 * Flow unit testing framework
 *
 * This is an *extremely* lightweight framework for writing optionally asynchronous,
 * optionally randomized unit tests.
 *
 * Usage:
 *
 * TEST_CASE( "product/module/testcase" ) {
 *   double random_test_parameter = g_random->random01();
 *   ASSERT( something );
 *   return Void();
 * }
 *
 * In an `.actor.cpp` file, the body of a TEST_CASE is an actor (may contain `wait`, `state`, etc)
 * In a `.cpp` file, the body of a TEST_CASE is an ordinary function returning a Future<Void>
 *
 * Our tools for actually executing tests are external to flow (and use g_unittests to find test cases).
 * See the `UnitTestWorkload` class.
 */

#include "flow.h"

struct UnitTest {
	typedef Future<Void> (*TestFunction)();

	const char* name;
	const char* file;
	int line;
	TestFunction func;
	UnitTest* next;

	UnitTest(const char* name, const char* file, int line, TestFunction func);
};

struct UnitTestCollection {
	UnitTest* tests;
};

extern UnitTestCollection g_unittests;

#define APPEND(a, b) a##b

// FILE_UNIQUE_NAME(basename) expands to a name like basename456 if on line 456
#define FILE_UNIQUE_NAME1(name, line) APPEND(name, line)
#define FILE_UNIQUE_NAME(name) FILE_UNIQUE_NAME1(name, __LINE__)

#ifdef FLOW_DISABLE_UNIT_TESTS

#define TEST_CASE(name) static Future<Void> FILE_UNIQUE_NAME(disabled_testcase_func)()
#define ACTOR_TEST_CASE(actorname, name)

#else

#define TEST_CASE(name)                                                                                                \
	static Future<Void> FILE_UNIQUE_NAME(testcase_func)();                                                             \
	namespace {                                                                                                        \
	static UnitTest FILE_UNIQUE_NAME(testcase)(name, __FILE__, __LINE__, &FILE_UNIQUE_NAME(testcase_func));            \
	}                                                                                                                  \
	static Future<Void> FILE_UNIQUE_NAME(testcase_func)()

// ACTOR_TEST_CASE generated by actorcompiler; don't use directly
#define ACTOR_TEST_CASE(actorname, name)                                                                               \
	namespace {                                                                                                        \
	UnitTest APPEND(testcase_, actorname)(name, __FILE__, __LINE__, &actorname);                                       \
	}

#endif

#endif