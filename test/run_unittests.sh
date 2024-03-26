TEST_DIR="../cmake-build/test"

find "$TEST_DIR" -type f -executable -name "*unittest" | while read -r test_file; do
    echo "Running $test_file..."
    "$test_file"
    echo "Finished $test_file"
    echo "--------------------------------"
done
