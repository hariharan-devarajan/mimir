//
// Created by haridev on 2/16/22.
//

TEST_CASE("SingleWrite", "[process=" +
std::to_string(info
.comm_size) +
"]"
"[operation=single_write]"
"[request_size=type-fixed][repetition=1]"
"[file=1]") {
pretest();

SECTION("write to existing file") {
test::test_open(info
.existing_file.

c_str(), O_RDWR

);
REQUIRE(test::fh_orig
!= -1);
test::test_seek(0, SEEK_SET);
REQUIRE(test::status_orig
== 0);
test::test_write(info
.write_data.

data(), args

.request_size);
REQUIRE(test::size_written_orig
== args.request_size);

test::test_close();

REQUIRE(test::status_orig
== 0);
}

SECTION("write to new  file") {
test::test_open(info
.new_file.

c_str(), O_WRONLY

| O_CREAT | O_EXCL, 0600);
REQUIRE(test::fh_orig
!= -1);
test::test_write(info
.write_data.

data(), args

.request_size);
REQUIRE(test::size_written_orig
== args.request_size);

test::test_close();

REQUIRE(test::status_orig
== 0);
REQUIRE(fs::file_size(info.new_file)
== test::size_written_orig);
}

SECTION("write to existing file with truncate") {
test::test_open(info
.existing_file.

c_str(), O_WRONLY

| O_TRUNC);
REQUIRE(test::fh_orig
!= -1);
test::test_write(info
.write_data.

data(), args

.request_size);
REQUIRE(test::size_written_orig
== args.request_size);

test::test_close();

REQUIRE(test::status_orig
== 0);
REQUIRE(fs::file_size(info.existing_file)
== test::size_written_orig);
}

SECTION("write to existing file at the end") {
test::test_open(info
.existing_file.

c_str(), O_RDWR

);
REQUIRE(test::fh_orig
!= -1);
test::test_seek(0, SEEK_END);
REQUIRE(((size_t)
test::status_orig) ==
args.
request_size *info
.num_iterations);
test::test_write(info
.write_data.

data(), args

.request_size);
REQUIRE(test::size_written_orig
== args.request_size);

test::test_close();

REQUIRE(test::status_orig
== 0);
REQUIRE(fs::file_size(info.existing_file)
==
test::size_written_orig + args.
request_size *info
.num_iterations);
}

SECTION("append to existing file") {
auto existing_size = fs::file_size(info.existing_file);
test::test_open(info
.existing_file.

c_str(), O_RDWR

| O_APPEND);
REQUIRE(test::fh_orig
!= -1);
test::test_write(info
.write_data.

data(), args

.request_size);
REQUIRE(test::size_written_orig
== args.request_size);

test::test_close();

REQUIRE(test::status_orig
== 0);
REQUIRE(fs::file_size(info.existing_file)
==
existing_size + test::size_written_orig);
}

SECTION("append to new file") {
test::test_open(info
.new_file.

c_str(), O_WRONLY

| O_CREAT | O_EXCL, 0600);
REQUIRE(test::fh_orig
!= -1);
test::test_write(info
.write_data.

data(), args

.request_size);
REQUIRE(test::size_written_orig
== args.request_size);

test::test_close();

REQUIRE(test::status_orig
== 0);
REQUIRE(fs::file_size(info.new_file)
== test::size_written_orig);
}

posttest();

}