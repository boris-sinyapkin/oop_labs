#include "pch.h"
#include <iostream>
#include <map>
#include <vector>
#include <exception>
#include <string>

using std::string;
using std::cout;
using std::endl;


			     /*stop name, stop time*/
typedef std::map<const char*, const char*> timetable;

class route
{
public:
	route(const char *init_route, timetable init_tt ) : 
		route_name(init_route), 
		transport_amount(0),
		route_tt(init_tt) {}

	const char *route_name;

	unsigned get_amount() { return transport_amount; }
	void operator++() { transport_amount++; }
	void operator--() { transport_amount--; }
	void get_arrived_time(const char *stop_name, const char *cur_time)
	{
		try
		{
			if (route_tt.find(stop_name) == route_tt.end()) throw(std::out_of_range(""));

			int diff = time_to_i(route_tt[stop_name]) - time_to_i(cur_time);

			if (diff < 0) 
				throw(std::exception("Transport passed this stop.\n"));
			else
			{
				cout << "Transport will arrived in " << diff / 60
													<< " hour(s) "
													<< diff % 60
													<< " minutes." << endl;
			}
		}
		catch (const std::out_of_range&)
		{
			cout << "This stop is not in the route." << endl;
		}
		catch (const std::exception& e)
		{
			cout << e.what() << endl;
		}
	}

private:
	unsigned transport_amount;
	timetable route_tt;

	//HH:MM
	int time_to_i(const char *time_str)
	{
		string temp_str(time_str);
		string hour = temp_str.substr(0, temp_str.find(":"));
		string minute = temp_str.substr(temp_str.find(":") + 1, temp_str.length());
		return std::atoi(hour.c_str()) * 60 + std::atoi(minute.c_str());
	}
};

class park;

class transport
{
public:
	transport(const char *numb, park *ap) : 
		cur_route(0), number(numb), appended_park(ap) {}

	transport(const char *numb, route *t_route, park* ap) : 
		cur_route(t_route), number(numb), appended_park(ap)
	{
		cur_route->operator++();
	}
	~transport()  {} 
	const char* get_number() { return number; }

	bool operator==(transport& b) 
	{
		return (*this).get_number() == b.get_number() ? true : false;
	}

private:
	transport() {}
	park *appended_park;
	route *cur_route;
	const char *number;
};

class bus : public transport 
{
public:
	bus(const char *numb, park *ap) : 
		transport(numb, ap) {}
	bus(const char *numb, route *t_route, park* ap) :
		transport(numb, t_route, ap) {}

};
class tram : public transport 
{
public:
	tram(const char *numb, park *ap) :
		transport(numb, ap) {}
	tram(const char *numb, route *t_route, park* ap) :
		transport(numb, t_route, ap) {}
};

/*number, class trnsport*/
typedef std::pair<const char*, transport> t_id;

class alert
{
public:
	alert(const char *at_num) : t_num(at_num) {}
	~alert() {}

	virtual void show_alert() = 0;

protected:
	const char *t_num;
};

struct crash : public alert 
{
	crash(const char *at_num) : alert(at_num) {}
	void show_alert()
	{
		cout << "type: " << typeid(*this).name() << ", registration mark: " << t_num << endl;
	}
};

struct delay : public alert 
{
	delay(const char *at_num) : alert(at_num) {}
	void show_alert()
	{
		cout << "type: " << typeid(*this).name() << ", registration mark: " << t_num << endl;
	}
};


class traffic_controller
{
public:
	template<typename alert_type>
	void raise_alert(const char *t_num)
	{
		alert_type *new_alert = new alert_type(t_num);
		alerts.push_back(new_alert);
	}

	void show_alerts()
	{
		if (alerts.empty()) cout << "Alerts empty." << endl;
		else
		{
			for (auto i = alerts.begin(); i != alerts.end(); i++) (*i)->show_alert();
		}
	}

	~traffic_controller()
	{
		for (auto i = alerts.begin(); i != alerts.end(); i++) delete (*i);
	}

private:
	std::vector<alert*> alerts;
};

class park
{
public:
	traffic_controller tc;

	park(const char *reg_mark) : park_name(reg_mark) {}
	~park() {}

	const char *park_name;

	void add_vehicle(transport vehicle)
	{
		t_map.insert(t_id(vehicle.get_number(), vehicle));
	}
	bool is_belongs(transport vehicle)
	{
		if (t_map.find(vehicle.get_number()) == t_map.end()) return false;
		else return true;
	}
	bool is_belongs(const char *transport_number)
	{
		if (t_map.find(transport_number) == t_map.end()) return false;
		else return true;
	}
	size_t park_size() { t_map.size(); }

private:
	/*number, class transport*/
	std::map<const char*, transport> t_map;
};

class monitor
{
public:
	void add_route(route *new_route) { routes[new_route->route_name] = new_route; }
	void add_park(park *new_park) { parks[new_park->park_name] = new_park; }

	void route_size(const char *route_name)
	{
		try
		{
		    if (routes.find(route_name) == routes.end()) throw(std::out_of_range(""));
			cout << "Route \"" << route_name << "\" size: " << routes[route_name]->get_amount();
		}
		catch (const std::exception&)
		{
			cout << "Route \"" << route_name << "\" does not exist." << endl;
		}
	}
	void is_belongs_to_park(const char *transport_number, const char *park_name)
	{
		try
		{
			if (parks.find(park_name) == parks.end()) throw(std::out_of_range(""));

			cout << "Belongs \"" << transport_number
				<< "\" to \"" << park_name << "\" ? : "
				<< parks[park_name]->is_belongs(transport_number);
		}
		catch (const std::out_of_range&)
		{
			cout << "Park \"" << park_name << "\" does not exist." << endl;
		}

	}
	void get_diff_time(const char *route_name, const char *stop_name, const char *cur_time)
	{
		try
		{
			if (routes.find(route_name) == routes.end()) throw(std::out_of_range(""));
			routes[route_name]->get_arrived_time(stop_name, cur_time);
		}
		catch (const std::exception&)
		{
			cout << "Route \"" << route_name << "\" does not exist." << endl;
		}
	}
	void show_alerts(const char *park_name)
	{
		try
		{
			if (parks.find(park_name) == parks.end()) throw(std::out_of_range(""));
			parks[park_name]->tc.show_alerts();
		}
		catch (const std::exception&)
		{
			cout << "Park \"" << park_name << "\" does not exist." << endl;
		}
	}
private:
			/*park name*/
	std::map<const char*, route*> routes;
	         /*route name*/
	std::map<const char*, park*> parks;
};

int main()
{
	route my_route("highway", { {"SaintP", "10:00"}, {"Moscow", "17:00"}, {"Voronezh", "20:00"} });

	park my_park("boris park");
	my_park.add_vehicle(bus("bus1", &my_route, &my_park));
	my_park.add_vehicle(bus("bus2", &my_route, &my_park));
	my_park.add_vehicle(bus("tram1", &my_route, &my_park));

	monitor my_mon;
	my_mon.add_park(&my_park);
	my_mon.add_route(&my_route);

	my_mon.get_diff_time("highway", "Voronezh", "9:00");
      //my_park.tc.raise_alert<crash>("bus1");
	  //my_park.tc.raise_alert<delay>("bus2");
	  //my_mon.show_alerts(my_park.park_name);


}




