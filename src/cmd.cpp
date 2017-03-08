#include <vector>
#include <string>

#include <boost/algorithm/string.hpp>

#include "config.hpp"
#include "common.hpp"
#include "db.hpp"
#include "utility.cpp"

NAMESPACE_BEGIN(monoco)
using std::vector;
using std::shared_ptr;
using utility::args_to_zls;
using errs::error;
using errs::print;

bool
is_valid(const string& cmd_name, size_t counts,
		 const vector<string>& args, bool loose)
{
	bool valid = false;
	if (loose && args.size() >= counts)
		valid = true;
	if (!loose)
		valid = args.size() == counts;
	if (boost::iequals(args[0], cmd_name)) {
		if (!valid) {
			errs::error("Invaild arguments");
			return false;
		}
		return true;
	}
	return false;
}

int
list_cmd(shared_ptr<mdb> db, vector<string>& args)
{
	vector<zl_entry> zls;

	if (is_valid("flushall", 1, args, false)) {
		db->_dict->clear();
		db->_exprs->clear();
		return 1;
	}

	if (is_valid("type", 2, args, false)) {
		print(db->type(args[1]));
		return 0;
	}

	if (is_valid("exists", 2, args, false)) {
		print(db->exists(args[1]));
		return 0;
	}
	
	if (is_valid("lindex", 3, args, false)) {
		auto res = db->lindex(args[1], std::stoull(args.back()));
		if (res.empty()) {
			errs::error(args[1], "not found");
			return -1;
		}
		print(res);
		return 0;
	}

	if (is_valid("linsert", 5, args, false)) {
		zls.clear();
		utility::args_to_zls(args.begin() + 3, args.end(),
							 zls);
		bool before = false;
		if (boost::iequals("before", args[2]))
			before = true;
		else if (boost::iequals("before", args[2]))
			;
		else {
			error("invalid arguments");
			return -1;
		}

		return db->linsert(args[1], before, zls[0], zls[1]);
	}

	if (is_valid("llen", 2, args, false)) {
		auto sz = db->llen(args[1]);
		if (sz == types::size_t_max) {
			return -1;
		}
		else {
			print(sz);
			return 0;
		}
	}

	if (is_valid("lpop", 2, args, false)) {
		auto res = db->lpop(args[1]);
		if (res.empty()) {
			return -1;
		}
		print(res);
		return 1;
	}

	if (is_valid("lpush", 3, args, true)) {
		zls.clear();
		args_to_zls(args.begin() + 2, args.end(), zls);
		return db->lpush(args[1],zls);
	}

	if (is_valid("rpop", 2, args, false)) {
		auto res = db->rpop(args[1]);
		if (res.empty()) {
			return -1;
		}
		print(res);
		return 1;
	}

	if (is_valid("rpush", 3, args, true)) {
		zls.clear();
		args_to_zls(args.begin() + 2, args.end(), zls);
		return db->rpush(args[1],zls);
	}

	if (is_valid("lpushx", 3, args, false)) {
		return db->lpushx(args[1], args[2]);
	}

	if (is_valid("rpushx", 3, args, false)) {
	    return db->rpushx(args[1], args[2]);
	}

	if (is_valid("lrange", 4, args, false)) {
		vector<zl_entry> res;
		int ret = db->lrange(res, args[1], std::stoull(args[2]),
							 std::stoull(args[3]));
		if (ret != 0) {
			return -1;
		}

		for (auto && ze : res) {
			print(ze);
		}
		return 0;
	}

	if (is_valid("lrem", 4, args, false)) {
		zls.clear();
		args_to_zls(args.begin() + 3, args.end(), zls);
		return db->lrem(args[1], std::stoull(args[2]),
					   zls.front());
	}

	if (is_valid("lset", 4, args, false)) {
		zls.clear();
		args_to_zls(args.begin() + 3, args.end(), zls);
		return db->lset(args[1], std::stoull(args[2]), zls.front());
	}

	if (is_valid("ltrim", 4, args, false)) {
		return db->ltrim(args[1], std::stoull(args[2]),
						std::stoull(args[3]));
	}

	if (is_valid("sadd", 3, args, true)) {
		zls.clear();
		args_to_zls(args.begin() + 2, args.end(), zls);
		return db->sadd(args[1], zls);

	}

	if (is_valid("scard", 2, args, false)) {
		print(db->scard(args[1]));
		return 0;
	}

	if (is_valid("sismember", 3, args, false)) {
		zls.clear();
		args_to_zls(args.begin() + 2, args.end(), zls);
		print(db->sismember(args[1], zls[0]));
		return 0;
	}

	if (is_valid("srem", 3, args, false)) {
		zls.clear();
		args_to_zls(args.begin() + 2, args.end(), zls);
		return db->srem(args[1], zls[0]);
	}
	
	if (is_valid("smembers", 2, args, false)) {
		vector<zl_entry> res;
		int ret = db->smembers(res, args[1]);
		if (ret != 0) {
			return -1;
		}

		for (auto && ze : res) {
			std::cout << ze << " ";
		}
		return 0;
	}

	if (is_valid("sdiff", 3, args, false)) {
		vector<zl_entry> res;
		int ret = db->sdiff(res, args[1], args[2]);
		if (ret != 0) {
			return -1;
		}

		for (auto && ze : res) {
			std::cout << ze << " ";
		}
		print("");
		return 0;
	}

	if (is_valid("sunion", 3, args, false)) {
		vector<zl_entry> res;
		int ret = db->sunion(res, args[1], args[2]);
		if (ret != 0) {
			return -1;
		}

		for (auto && ze : res) {
			std::cout << ze << " ";
		}
		print("");
		return 0;
	}

	if (is_valid("sinter", 3, args, false)) {
		vector<zl_entry> res;
		int ret = db->sinter(res, args[1], args[2]);
		if (ret != 0) {
			return -1;
		}

		for (auto && ze : res) {
			std::cout << ze << " ";
		}
		print("");
		return 0;
	}

	if (is_valid("strlen", 2, args, false)) {
		print(db->strlen(args[1]));
		return 0;
	}

	if (is_valid("set", 3, args, false)) {
		zls.clear();
		args_to_zls(args.begin() + 2, args.end(),
					zls);
		return db->set(args[1], zls[0]);
	}

	if (is_valid("append", 3, args, false)) {
		return db->append(args[1], args[2]);
	}

	if (is_valid("get", 2, args, false)) {
		print(db->get(args[1]));
		return 0;
	}

	if (is_valid("incrby", 3, args, false)) {
		return db->incr_by(args[1], std::stold(args[2]));
	}

	if (is_valid("decrby", 3, args, false)) {
		print(db->decr_by(args[1], std::stold(args[2])));
	}

	if (is_valid("decr", 2, args, false)) {
		return db->decr(args[1]);
	}

	if (is_valid("incr", 2, args, false)) {
		return db->incr(args[1]);
	}

	auto handle_str = [](const string & str)
		{
			string cp = str;
			try{
			if (str.front() == '\'' ||
				str.front() == '"') {
				cp.erase(0, 1);
				cp.erase(cp.size() - 1);
			}
			} catch (...) {
				return str;
			}

			return cp;
		};
	
	if (is_valid("zadd", 4, args, true)) {
		if (args.size() % 2 != 0) {
			error("invalid arguments");
			return -1;
		}

		vector<std::pair<string, long double>> ps;

		std::pair<string, long double> p;
		for (size_t i = 2; i != args.size(); ++i) {
			if (i % 2 == 0) {
				p.second = stold(args[i]);
			}
			else{
				p.first = args[i];
				ps.push_back(p);
			}
		}

		return(db->zadd(args[1], ps));
	}

	if (is_valid("zcard", 2, args, false)) {
		print(db->zcard(args[1]));
		return 0;
	}

	if (is_valid("zscore", 3, args, false)) {
		print(db->zscore(args[1], args[2]));
	}

	if (is_valid("zrank", 3, args, false)) {
		print(db->zrank(args[1], args[2]));
		return 0;
	}

	if (is_valid("zrem", 3, args, false)) {
		return db->zrem(args[1], args[2]);
	}
	
	if (is_valid("zcount", 4, args, false)) {
		print(db->zcount(args[1], std::stold(args[2]),
						 std::stold(args[3])));
		return 0;
	}

	if (is_valid("zincrby", 4, args, false)) {
		return db->zincr_by(args[1], args[2],
						 std::stold(args[3]));
	}
	
	if (is_valid("zrange", 4, args, true)) {
		vector<std::pair<string, long double>> res;
		int ret = db->zrange(res, args[1], std::stold(args[2]),
							 std::stold(args[3]));
		if (ret != 0) {
			return -1;
		}

		for (auto && ze : res) {
			print(ze.first, " ", ze.second);
		}

		return 0;
	}

	if (is_valid("hcard", 2, args, false)) {
		print(db->hcard(args[1]));
		return 0;
	}

	if (is_valid("hdel", 3, args, false)) {
		return db->hdel(args[1], args[2]);
	}

	if (is_valid("hget", 3, args, false)) {
		print(db->hget(args[1], args[2]));
	}

	if (is_valid("hexists", 3, args, false)) {
		print(db->hexists(args[1], args[2]));
		return 0;
	}

	if (is_valid("hset", 4, args, false)) {
		zls.clear();
		print(args[3]);
		args_to_zls(args.begin() + 3, args.end(), zls);
		return db->hset(args[1], args[2], zls[0]);
	}
	
	if (is_valid("hgetall", 2, args, false)) {
		vector<std::pair<string, zl_entry>> res;
		int ret = db->hgetall(res, args[1]);
		if (ret != 0) {
			return -1;
		}

		for (auto && ze : res) {
			print(ze.first, " ", ze.second);
		}
		return 0;
	}

	if (is_valid("hvals", 2, args, false)) {
		vector<zl_entry> res;
		int ret = db->hvals(res, args[1]);
		if (ret != 0) {
			return -1;
		}

		for (auto && ze : res) {
			print(ze);
		}
		return 0;
	}

	if (is_valid("hkeys", 2, args, false)) {
		vector<string> res;
		int ret = db->hkeys(res, args[1]);
		if (ret != 0) {
			return -1;
		}

		for (auto && ze : res) {
			print(ze);
		}
		return 0;
	}

	print("Invalid arguments");
	return -1;
}

int
handle_cmds(shared_ptr<mdb> db, vector<string>& args)
{
	if (args.size() <= 1)
		return -1;
	try {
	if(args[1].front() == '.' ||
	   args[1].front() == '"') {
		args[1].erase(0, 1);
		args[1].erase(args[1].size() -1);
	}
	}
	catch(...) {
		error("invalid arguments");
	}
	
	return list_cmd(db, args);
}

NAMESPACE_END(monoco)
