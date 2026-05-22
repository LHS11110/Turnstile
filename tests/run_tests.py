#!/usr/bin/env python3
import subprocess
import sys
import os

# Define test cases with their expected outcomes and potential error messages
TEST_CASES = [
    {
        "filename": "valid_identity.txt",
        "expected_success": True,
        "expected_error": None
    },
    {
        "filename": "valid_commutation.txt",
        "expected_success": True,
        "expected_error": None
    },
    {
        "filename": "valid_distributivity.txt",
        "expected_success": True,
        "expected_error": None
    },
    {
        "filename": "valid_contraposition.txt",
        "expected_success": True,
        "expected_error": None
    },
    {
        "filename": "invalid_indent.txt",
        "expected_success": False,
        "expected_error": "Line 3: Indent level does not match"
    },
    {
        "filename": "invalid_proof.txt",
        "expected_success": False,
        "expected_error": "Line 4: Invalid proof"
    },
    {
        "filename": "invalid_rule_premise.txt",
        "expected_success": False,
        "expected_error": "Line 2: Invalid proof: not enough premises on stack"
    },
    {
        "filename": "invalid_no_theorem.txt",
        "expected_success": False,
        "expected_error": "Line 1: Provable cannot be defined in a theorem context"
    }
]

def run_test(binary_path, test_dir, case):
    filepath = os.path.join(test_dir, case["filename"])
    if not os.path.exists(filepath):
        print(f"[-] Test file {case['filename']} not found!")
        return False

    # Execute build/turnstile < tests/filename
    try:
        with open(filepath, "r") as f:
            result = subprocess.run(
                [binary_path],
                stdin=f,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                text=True,
                timeout=5
            )
    except subprocess.TimeoutExpired:
        print(f"[-] TEST FAILED: {case['filename']} (Timeout)")
        return False
    except Exception as e:
        print(f"[-] TEST FAILED: {case['filename']} (Execution error: {e})")
        return False

    output = result.stdout + result.stderr

    # Determine if there was any error reported
    has_error = "[ERROR]" in output

    if case["expected_success"]:
        if has_error:
            print(f"[-] TEST FAILED: {case['filename']} (Expected success, but got error)")
            print(f"    Output:\n{output.strip()}")
            return False
        else:
            print(f"[+] TEST PASSED: {case['filename']} (Successfully verified proof)")
            return True
    else:
        if not has_error:
            print(f"[-] TEST FAILED: {case['filename']} (Expected error, but proof succeeded/no error reported)")
            return False
        
        # Verify the expected error message is present
        if case["expected_error"] not in output:
            print(f"[-] TEST FAILED: {case['filename']} (Got error, but mismatching message)")
            print(f"    Expected substring: '{case['expected_error']}'")
            print(f"    Actual output:\n{output.strip()}")
            return False
        else:
            print(f"[+] TEST PASSED: {case['filename']} (Failed as expected with: '{case['expected_error']}')")
            return True

def main():
    # Find the binary
    script_dir = os.path.dirname(os.path.abspath(__file__))
    project_root = os.path.dirname(script_dir)
    binary_path = os.path.join(project_root, "build", "turnstile")

    if not os.path.exists(binary_path):
        print(f"[-] Turnstile binary not found at {binary_path}!")
        print("    Please run 'cmake --build build' first to compile the binary.")
        sys.exit(1)

    print("=========================================")
    print("  Running Turnstile Proof Assistant Tests ")
    print("=========================================")
    
    passed_count = 0
    for case in TEST_CASES:
        if run_test(binary_path, script_dir, case):
            passed_count += 1

    print("=========================================")
    print(f"Results: {passed_count}/{len(TEST_CASES)} tests passed.")
    print("=========================================")

    if passed_count == len(TEST_CASES):
        print("[+] All tests passed successfully!")
        sys.exit(0)
    else:
        print("[-] Some tests failed.")
        sys.exit(1)

if __name__ == "__main__":
    main()
