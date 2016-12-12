/*
 * attribute_types.hpp
 *
 *  Created on: Nov 21, 2009
 *      Author: Troy
 */
#include <cstddef>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian_calendar.hpp>
#include <boost/date_time/time_facet.hpp>
#include <string>
#include <ctime>
#include <cstdio>
#include <cstddef>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>

#ifndef ATTRIBUTE_TYPES_HPP_
#define ATTRIBUTE_TYPES_HPP_
typedef long int_type; // takes a lot of bits to represent 11 digits


/* takes a lot bits to represent 22 digits
 * also might want to make this an integer value since it represents mostly money
 * floating point numbers should not be used to represent exact figures rather use an
 * exact number of pennies to represent money
 */
typedef double float_type;

static const unsigned int DATE_SIZE = 20; // 19 plus 1 for the null terminator
static const unsigned int COMMENT_SIZE = 121; // 120 plus 1 for the null terminator

typedef char comment_type[COMMENT_SIZE]; // save some typing;

typedef std::time_t date_type; // might want to change date_type to numeric latter

typedef char c_name_t[26];
typedef char c_address_t[51];
typedef char c_phone_t[21];
typedef char c_mktseg_t[16];

typedef char l_returnflag_t[3];
typedef char l_linestatus_t[3];
typedef char l_shipinstruct_t[21];
typedef char l_shipmode_t[11];

typedef char n_name_t[16];
typedef char r_name_t[51];

typedef char o_orderstatus_t[3];
typedef char o_oderpriority_t[21];
typedef char o_orderclerk_t[21];

typedef char p_name_t[61];
typedef char p_mfgr_t[16];
typedef char p_brand_t[11];
typedef char p_type_t[31];
typedef char p_container_t[16];

typedef char s_name_t[26];
typedef char s_address_t[51];
typedef char s_phone_t[31];

typedef char b_countrycode_t[4];

#define INT_DISK_SIZE(N) 11
#define FLOAT_DISK_SIZE(N) 22
#define DATE_DISK_SIZE(N) 19
#define BOOL_DISK_SIZE(N) 2

static const std::size_t NEWLINE_DISK_SIZE = 2;
static const std::string NEW_LINE = "\n";

static int_type deserialize_int(const char* const stream,
		std::size_t& bytes_read) {
	std::size_t start = bytes_read;
	std::size_t end = start + INT_DISK_SIZE(int_type);

	std::string buffer(&stream[start], &stream[end]);
	boost::algorithm::trim_right(buffer);
	bytes_read += INT_DISK_SIZE(int_type);
	return boost::lexical_cast<int_type>(buffer);
}

static bool deserialize_bool(const char* const stream, std::size_t& bytes_read) {
	std::size_t start = bytes_read;
	std::size_t end = start + BOOL_DISK_SIZE(bool);

	std::string buffer(&stream[start], &stream[end]);
	boost::algorithm::trim_right(buffer);
	bytes_read += BOOL_DISK_SIZE(bool);
	return boost::lexical_cast<bool>(buffer);
}

static std::string serialize_bool(bool b) {
	std::string s = boost::lexical_cast<std::string>(b);
	int spaces = BOOL_DISK_SIZE(bool) - s.size();
	if (spaces > 0) {
		s.insert(s.end(), spaces, ' ');
	}
	return s;
}

static float_type deserialize_float(const char* const stream,
		std::size_t& bytes_read) {
	std::size_t start = bytes_read;
	std::size_t end = start + FLOAT_DISK_SIZE(int_type);
	std::string buffer(&stream[start], &stream[end]);
	boost::algorithm::trim_right(buffer);
	bytes_read += FLOAT_DISK_SIZE(float_type);
	return boost::lexical_cast<float_type>(buffer);
}

static void deserialize_char(const char* const stream, char* dest,
		std::size_t& bytes_read, std::size_t bytes_to_read) {
	std::size_t start = bytes_read;
	std::size_t end = start + bytes_to_read;
	std::string buffer(&stream[start], &stream[end]);
	boost::algorithm::trim_right(buffer);
	std::copy(buffer.begin(), buffer.end(), &dest[0]);
	dest[buffer.size()] = '\0';
	bytes_read += bytes_to_read;
}

static boost::posix_time::ptime ymd_string_to_date(
		const char* const date_string) {

	int month, day, year, hour, minute, second;

	char s[6];

	std::sscanf(date_string, "%d%c%d%c%d%c%d%c%d%c%d%c", &year, &s[0], &month,
			&s[1], &day, &s[2], &hour, &s[3], &minute, &s[4], &second, &s[5]);

	boost::gregorian::date d(year, month, day);
	boost::posix_time::time_duration td(hour, minute, second, 0);
	boost::posix_time::ptime time(d, td);
	return time;
}

static boost::posix_time::ptime mdy_string_to_date(
		const char* const date_string) {

	int month, day, year, hour, minute, second;

	char s[6];

	std::sscanf(date_string, "%d%c%d%c%d%c%d%c%d%c%d%c", &month, &s[0], &day,
			&s[1], &year, &s[2], &hour, &s[3], &minute, &s[4], &second, &s[5]);

	boost::gregorian::date d(year, month, day);
	boost::posix_time::time_duration td(hour, minute, second, 0);
	boost::posix_time::ptime time(d, td);
	return time;
}

static std::string serialize_int(const int_type& i) {
	std::string r = boost::lexical_cast<std::string>(i);
	int spaces = INT_DISK_SIZE(int_type) - r.size();
	if (spaces > 0) {
		r.insert(r.end(), spaces, ' ');
	}
	return r;
}

static std::string serialize_char(const char* const str, std::size_t size) {
	std::string r(str);
	int spaces = size - r.size();
	if (spaces > 0)
		r.insert(r.end(), spaces, ' ');
	return r;

}

static std::string serialize_float(const float_type& f) {
	std::string r = boost::lexical_cast<std::string>(f);
	int spaces = FLOAT_DISK_SIZE(int_type) - r.size();
	if (spaces > 0) {
		r.insert(r.end(), spaces, ' ');
	}
	return r;
}

static std::string serialize_newline() {
	return NEW_LINE;
}

static void pad_date(std::string& date_string) {
	int spaces = DATE_DISK_SIZE(date_type) - date_string.size();
	if (spaces > 0)
		date_string.insert(date_string.end(), spaces, ' ');
}

static std::string serialize_ymd_date(date_type d) {
	char date[19];

	std::tm* t = std::localtime(&d);

	std::sprintf(date, "%d-%d-%d %d:%d:%d", t->tm_year + 1900, t->tm_mon + 1,
			t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	std::string s(date);
	pad_date(s);
	return s;
}

static std::string serialize_mdy_date(date_type d) {
	char date[19];

	std::tm* t = std::localtime(&d);

	std::sprintf(date, "%d/%d/%d %d:%d:%d", t->tm_mon + 1, t->tm_mday,
			t->tm_year + 1900, t->tm_hour, t->tm_min, t->tm_sec);
	std::string s(date);
	pad_date(s);
	return s;
}

static date_type deserialize_ymd_date(const char* const stream,
		std::size_t& bytes_read) {
	std::size_t start = bytes_read;
	std::size_t end = start + DATE_DISK_SIZE(date_type);
	char buffer[DATE_DISK_SIZE(date_type)];
	std::copy(&stream[start], &stream[end], &buffer[0]);
	buffer[DATE_DISK_SIZE(date_type) - 1] = '\0';
	boost::posix_time::ptime time = ymd_string_to_date(buffer);
	std::tm ts = boost::posix_time::to_tm(time);
	bytes_read += DATE_DISK_SIZE(date_type);
	return std::mktime(&ts);
}

static date_type deserialize_mdy_date(const char* const stream,
		std::size_t& bytes_read) {
	std::size_t start = bytes_read;
	std::size_t end = start + DATE_DISK_SIZE(date_type);
	char buffer[DATE_DISK_SIZE(date_type)];
	std::copy(&stream[start], &stream[end], &buffer[0]);
	buffer[DATE_DISK_SIZE(date_type) - 1] = '\0';
	boost::posix_time::ptime time = mdy_string_to_date(buffer);
	std::tm ts = boost::posix_time::to_tm(time);
	bytes_read += DATE_DISK_SIZE(date_type);
	return std::mktime(&ts);
}

#endif /* ATTRIBUTE_TYPES_HPP_ */
