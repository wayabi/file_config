#include <stdio.h>
#include <stdlib.h>

#include <iostream>

#include <boost/thread.hpp>
#include <boost/asio.hpp>

#include "file_config.h"

using namespace std;

int main(int argc, char** argv){
	boost::asio::io_service ios_;
	boost::asio::io_service::work work_(ios_);

	file_config fc("./config", ',');
	fc.set_file_checksum("_checksum_");
	fc.set_reload_interval(1);
	fc.start_reload_loop(&ios_);

	boost::thread_group tg;
	tg.create_thread(boost::bind(&boost::asio::io_service::run, &ios_));

	usleep(200*1000);
	while(true){
		if(fc.get_string("string") == nullptr) break;
		if(fc.get_int("int") == nullptr) break;
		if(fc.get_double("double") == nullptr) break;
		usleep(10*1000);
	}

	cout << "end" << endl;

	ios_.stop();
	tg.join_all();

	return 0;
}
