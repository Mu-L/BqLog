#!/usr/bin/env python3
# Copyright (C) 2025 Tencent.
# BQLOG is licensed under the Apache License, Version 2.0.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.

"""
Test entry point for BqLog Python tests.
Aligned with Java test_main.
"""

import sys
import os

# Add test source to sys.path BEFORE any bq imports, so that
# extend_path in bq/__init__.py can discover the test sub-package.
# Directory structure: test/python/src/bq/test/test_main.py
# We need test/python/src on sys.path.
script_dir = os.path.dirname(os.path.abspath(__file__))
test_src = os.path.abspath(os.path.join(script_dir, "..", ".."))
sys.path.insert(0, test_src)

from bq.log import log
from bq.defs.log_level import log_level
from bq.test.test_manager import test_manager
from bq.test.test_log_1 import test_log_1
from bq.test.test_log_2 import test_log_2


def main():
    print("Running Python Wrapper Tests...")
    try:
        # Verify library load
        version = log.get_version()
        print("BqLog Version: " + (version or ""))
        if not version:
            raise RuntimeError("Failed to get version")

        test_manager.add_test(test_log_1("Test Log Basic"))
        test_manager.add_test(test_log_2("Test Log MultiThread"))
        success = test_manager.test()

        if success:
            log.console(log_level.info, "--------------------------------")
            log.console(log_level.info, "CONGRATULATION!!! ALL TEST CASES IS PASSED")
            log.console(log_level.info, "--------------------------------")
            return 0
        else:
            log.console(log_level.error, "--------------------------------")
            log.console(log_level.error, "SORRY!!! TEST CASES FAILED")
            log.console(log_level.error, "--------------------------------")
            return -1
    except Exception as e:
        import traceback
        traceback.print_exc()
        log.console(log_level.error, "--------------------------------")
        log.console(log_level.error, "SORRY!!! TEST CASES FAILED")
        log.console(log_level.error, "--------------------------------")
        return -1


if __name__ == "__main__":
    sys.exit(main())
