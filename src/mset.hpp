#ifndef __MSET_HPP_
#define __MSET_HPP_

#include <vector>
#include <unordered_set>
#include <functional>

#include "mbj.hpp"
#include "common.hpp"
#include "zl_entry.hpp"

NAMESPACE_BEGIN(monoco)

class mset : public mbj
{
public:
	typedef monoco::zl_entry    value_type;
	
	static constexpr uint8_t VEC_TAG = 1;
	static constexpr uint8_t SET_TAG = 2;
	uint8_t tag = VEC_TAG;
	
private:
	std::vector<value_type> _vec;
	std::unordered_set<value_type> _set;

	void _evolve()
		{
			if (tag == SET_TAG)
				return ;
			for (auto && ele: _vec)
				_set.emplace(ele);
			_vec.clear();
			tag = SET_TAG;
		}

	template <typename T>
	void try_evolve(const T& val)
		{
			if (_vec.size() > configs::mht_max_len)
				_evolve();
		}

public:

	virtual string type_name() const {return "mset";}
	
	template <typename T>
	void insert(const T& val)
		{
			try_evolve(val);
			
			if (tag == VEC_TAG)
				_vec.emplace_back(val);
			else
				_set.emplace(val);
		}

	template <typename T>
	void remove(const T& val)
		{
			if (tag == VEC_TAG) {
				auto iter = std::find(_vec.begin(), _vec.end(), val);
				if (iter != _vec.end())
					_vec.erase(iter);
			}
			else {
				auto iter = _set.find(val);
				if (iter != _set.end())
					_set.erase(iter);
			}
		}

	virtual std::size_t size() const
		{return tag == VEC_TAG ? _vec.size() : _set.size();}

	void clear()
		{
			if (tag == VEC_TAG)
				_vec.clear();
			else
				_set.clear();
			tag = VEC_TAG;
		}

	template <typename T>
	bool exists(const T& val)
		{
			if (tag == VEC_TAG) {
				auto iter = std::find(_vec.begin(), _vec.end(),
									  zl_entry(val));
				return iter != _vec.end();
			}
			else {
				return _set.end() != _set.find(zl_entry(val));
			}
		}

	/*
	template <typename CON, typename MEM_FN>
	void getall(CON &con, MEM_FN mem_fun)
		{
			if (tag == VEC_TAG)
				for(auto &&ele : _vec)
					std::bind(mem_fun(con, ele.second);
			else
				for(auto &&ele : _set)
					mem_fun(con, ele.second);
		}
	*/
	template <typename CON>
	void getall(CON &con)
		{
			if (tag == VEC_TAG)
				for(auto &&ele : _vec)
					con.push_back(ele);
			else
				for(auto &&ele : _set)
					con.push_back(ele);
		}

	virtual std::ofstream&
	write_to(std::ofstream& os, boost::crc_32_type& crc) const
		{
			size_t holder;
			holder = types::hash_idx<mset>();
			fs::write_to(os, holder, crc);

			holder = size();
			fs::write_to(os, holder, crc);

			auto _write = [&](auto & ls)
				{
					for (auto && ele : ls) {
						ele.write_to(os, crc);
					}
				};
			if (tag == VEC_TAG)
				_write(_vec);
			else
				_write(_set);

			return os;
		}

	virtual std::ifstream&
	read_from(std::ifstream& is, boost::crc_32_type& crc)
		{
			size_t holder;
			fs::read_from(is, holder, crc);
			if (holder != types::hash_idx<mset>()) {
				errs::log("read mset error");
				return is;
			}

			// read size
			fs::read_from(is, holder, crc);

			for (size_t i = 0; i != holder; ++i) {
				zl_entry ze;
				ze.read_from(is, crc);
				this->insert(ze);
			}
			return is;
		}
	
};

template <>
void
mset::try_evolve<string>(const string& value)
{
	if (value.size() > configs::mht_max_size ||
		_vec.size() > configs::mht_max_len)
		_evolve();
}

NAMESPACE_BEGIN(types)
template <>
string type_name<mset>() {return "mstr";}

static const size_t M_SET = std::hash<type_index>{}(type_index(typeid(mset)));

NAMESPACE_END(types)

NAMESPACE_END(monoco)
#endif
