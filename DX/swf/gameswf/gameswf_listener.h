// gameswf_listener.h	-- Vitaly Alexeev <tishka92@yahoo.com>	2007

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.


#ifndef GAMESWF_LISTENER_H
#define GAMESWF_LISTENER_H

#include "gameswf/gameswf.h"
#include "gameswf/gameswf_function.h"
#include "base/container.h"
#include "base/utility.h"
#include "base/weak_ptr.h"

namespace gameswf
{

	struct listener : public ref_counted
	{

		// misc
		struct clone_listener : public array<gc_ptr<as_object> >
		{
			clone_listener(const array< weak_ptr<as_object> >& listeners)
			{
				for (int i = 0, n = listeners.size(); i < n; i++)
				{
					if (listeners[i] != NULL)
					{
						push_back(listeners[i].get_ptr());
					}
				}
			}
		};

		void add(as_object* listener);
		bool remove(as_object* listener);

		void notify(const event_id& ev);
		void notify(const tu_string& event_name, const fn_call& fn);
		void advance(float delta_time);

		void clear() { m_listeners.clear(); }
		as_object*	operator[](const tu_stringi& name) const;
		as_object*	operator[](int index) const;
		int	size() const;
		void enumerate(as_environment* env) const;

	private:

		array< weak_ptr<as_object> > m_listeners;

	};

}

#endif
