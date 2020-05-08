#include "SQLiteImpl.h"
#include <stdexcept>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <sstream>

namespace sql {

	static int Callback(void* ptr, int ac, char** av, char** column_name) 
	{
		auto* sqlite_impl = static_cast<SQLiteImpl*>(ptr);
		column_t col;
		
		for(size_t i = 0; i < ac; i++) {
			col.emplace_back(column_name[i], sqlite_impl->GetValue(av[i]));
		}

		sqlite_impl->table_.emplace_back(col);		
		return 0;
	}

	SQLiteImpl::SQLiteImpl(const std::string& file)
	{
		if(sqlite3_open(file.c_str(), &db_)) {
			throw std::runtime_error("Could not load database");
		}
	}

	SQLiteImpl::~SQLiteImpl()
	{
		sqlite3_close(db_);
	}

	bool SQLiteImpl::ExecuteString(const std::string& cmd)
	{
		table_.clear();
		error_.clear();
		char* errorMsg;
		
		if(!sqlite3_exec(db_, cmd.c_str(), Callback, this, &errorMsg)) {
			return true;
		}

		error_ = errorMsg;
		sqlite3_free(errorMsg);
		return false;	
	}

	sql::value_t SQLiteImpl::GetValue(const std::string& str) const
	{
		if (str.empty())
			return nullptr;

		int64_t intResult;
		double dblResult;

		std::stringstream stream;
		stream << str;
		stream >> intResult;
				
		if (!stream.fail())
			return intResult;

		stream >> dblResult;

		if (!stream.fail())
			return dblResult;
		
		return str;
	}

} // End namespace sql.
