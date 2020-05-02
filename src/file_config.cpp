#include "file_config.h"
#include <stdio.h>
#include <assert.h>

#include <iostream>
#include <string>

#include "Util.h"

using namespace std;

file_config::file_config(const char* path, char delimiter)
{
	set_file(path, delimiter);
	reload_interval_millisec_ = 0;
	ios_ = NULL;
}

void file_config::set_file(const char* path, char delimiter)
{
	path_file_ = string(path);
	delimiter_ = delimiter;
}

std::shared_ptr<std::string> file_config::get_string(const char* name)
{
	std::lock_guard<std::mutex> lock(mtx_);
	auto ite = map_.find(string(name));
	if(ite == map_.end()) return nullptr;
	return make_shared<string>(ite->second);
}

std::shared_ptr<int> file_config::get_int(const char* name)
{
	std::lock_guard<std::mutex> lock(mtx_);
	auto ite = map_.find(string(name));
	if(ite == map_.end()) return nullptr;
	return make_shared<int>(atoi(ite->second.c_str()));

}

std::shared_ptr<double> file_config::get_double(const char* name)
{
	std::lock_guard<std::mutex> lock(mtx_);
	auto ite = map_.find(string(name));
	if(ite == map_.end()) return nullptr;
	return make_shared<double>(atof(ite->second.c_str()));
}

void file_config::set_reload_interval(int millisec)
{
	reload_interval_millisec_ = millisec;
}

void file_config::reload()
{
	FILE* f;
	if((f = fopen(path_file_.c_str(), "r")) == NULL){
		return;
	}
	vector<char> buf;
	const int size_buf = 1024*16;
	buf.resize(size_buf);

	map_tmp_.clear();
	while(fgets(&buf[0], size_buf, f) != NULL){
		auto ss = Util::split(Util::trim(string(&buf[0])), delimiter_);
		if(ss.size() >= 2){
			map_tmp_[ss[0]] = ss[1];
			cout << ss[0] << delimiter_ << ss[1] << endl;
		}
	}
	cout << endl;
	fclose(f);

	{
		std::lock_guard<std::mutex> lock(mtx_);
		map_ = map_tmp_;
	}
}

void file_config::start_reload_loop(boost::asio::io_service* ios)
{
	ios_ = ios;
	set_reload_event();
}

void file_config::set_reload_event()
{
	assert(ios_ != NULL);
	if(reload_interval_millisec_ <= 0) return;

	timer_loop_ = make_shared<boost::asio::deadline_timer>(*ios_, boost::posix_time::milliseconds(reload_interval_millisec_));
	timer_loop_->async_wait([this](const boost::system::error_code& e){
			reload();
			set_reload_event();
			});
}
