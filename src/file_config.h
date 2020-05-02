#ifndef __U_FILE_CONFIG__
#define __U_FILE_CONFIG__

#include <stdio.h>

#include <string>
#include <map>
#include <mutex>
#include <memory>

#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

class file_config {
public:
	file_config(const char* path, char delimiter = ',');
	void set_file(const char* path, char delimiter = ',');
	std::shared_ptr<std::string> get_string(const char* name);
	std::shared_ptr<int> get_int(const char* name);
	std::shared_ptr<double> get_double(const char* name);
	void reload();
	void start_reload_loop(boost::asio::io_service* ios);
	void set_reload_interval(int millisec);
	void set_file_checksum(const char* checksum);

private:
	void set_reload_event();

private:
	std::string path_file_;
	char delimiter_;
	std::map<std::string, std::string> map_;
	std::map<std::string, std::string>  map_tmp_;
	int reload_interval_millisec_;
	std::mutex mtx_;
	boost::asio::io_service* ios_;
	std::shared_ptr<boost::asio::deadline_timer> timer_loop_;
	std::string checksum_;
};

#endif
