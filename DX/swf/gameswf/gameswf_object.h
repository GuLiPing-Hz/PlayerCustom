// gameswf_object.h	-- Thatcher Ulrich <tu@tulrich.com> 2003

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// A generic bag of attributes.	 Base-class for ActionScript
// script-defined objects.

#ifndef GAMESWF_OBJECT_H
#define GAMESWF_OBJECT_H

#include "gameswf/gameswf_value.h"
#include "gameswf/gameswf_environment.h"
#include "gameswf/gameswf_types.h"
#include "gameswf/gameswf_player.h"
#include "base/container.h"
#include "base/weak_ptr.h"
#include "base/tu_loadlib.h"

namespace gameswf
{
	void	as_object_addproperty(const fn_call& fn);
	void	as_object_registerclass( const fn_call& fn );
	void	as_object_hasownproperty(const fn_call& fn);
	void	as_object_watch(const fn_call& fn);
	void	as_object_unwatch(const fn_call& fn);
	void	as_object_dump(const fn_call& fn);
	void	as_global_object_ctor(const fn_call& fn);

	// flash9
	void	as_object_add_event_listener(const fn_call& fn);

	struct instance_info;

	struct as_object : public as_object_interface
	{
		// Unique id of a gameswf resource
		enum	{ m_class_id = AS_OBJECT };
		virtual bool is(int class_id) const
		{
			return m_class_id == class_id;
		}

		stringi_hash<as_value>	m_members;

		// It is used to register an event handler to be invoked when
		// a specified property of object changes.
		// TODO: create container based on stringi_hash<as_value>
		// watch should be coomon
		struct as_watch
		{
			as_watch() :	m_func(NULL)
			{
			}

			as_function* m_func;
			as_value m_user_data;
		};

		// primitive data type has no dynamic members
		stringi_hash<as_watch>*	m_watch;

		// it's used for passing new created object pointer to constructors chain
		weak_ptr<as_object> m_this_ptr;

		// We can place reference to __proto__ into members but it's used very often
		// so for optimization we place it into instance
		gc_ptr<as_object> m_proto;	// for optimization

		// pointer to owner
		weak_ptr<player> m_player;

		weak_ptr<instance_info> m_instance;

		as_object(player* player);
		virtual ~as_object();
		
		virtual const char*	to_string() { return "[object Object]"; }
		virtual double	to_number();
		virtual bool to_bool() { return true; }
		virtual const char*	type_of() { return "object"; }

		void	builtin_member(const tu_stringi& name, const as_value& val); 
		void	call_watcher(const tu_stringi& name, const as_value& old_val, as_value* new_val);
		virtual bool	set_member(const tu_stringi& name, const as_value& val);
		virtual bool	get_member(const tu_stringi& name, as_value* val);
		virtual bool	find_property( const tu_stringi & name, as_value * val );
		virtual bool	on_event(const event_id& id);
		virtual	void enumerate(as_environment* env);
		virtual as_object* get_proto() const;
		virtual bool watch(const tu_string& name, as_function* callback, const as_value& user_data);
		virtual bool unwatch(const tu_string& name);
		virtual void clear_refs(hash<as_object*, bool>* visited_objects, as_object* this_ptr);
		virtual void this_alive();
		virtual void alive() {}
		virtual void copy_to(as_object* target);
		virtual void dump(tu_string& tabs);
		virtual void dump();
		as_object* find_target(const as_value& target);
		virtual root* get_root() const;
		virtual as_environment*	get_environment() { return 0; }
		virtual void advance(float delta_time) { assert(0); }

		player* get_player() const { return m_player.get_ptr(); }
		bool	is_instance_of(const as_function* constructor) const;
		as_object* get_global() const;

		// get/set constructor of object
		bool get_ctor(as_value* val) const;
		void set_ctor(const as_value& val);
		void set_instance(instance_info * info);

		as_object* create_proto(const as_value& constructor);

	};

}

#endif
