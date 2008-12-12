/* === S Y N F I G ========================================================= */
/*!	\file widget_bonechooser.cpp
**	\brief Template File
**
**	$Id$
**
**	\legal
**	Copyright (c) 2002-2005 Robert B. Quattlebaum Jr., Adrian Bentley
**	Copyright (c) 2007 Chris Moore
**
**	This package is free software; you can redistribute it and/or
**	modify it under the terms of the GNU General Public License as
**	published by the Free Software Foundation; either version 2 of
**	the License, or (at your option) any later version.
**
**	This package is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
**	General Public License for more details.
**	\endlegal
*/
/* ========================================================================= */

/* === H E A D E R S ======================================================= */

#ifdef USING_PCH
#	include "pch.h"
#else
#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include "widget_bonechooser.h"
#include <gtkmm/menu.h>
#include "app.h"

#include "general.h"

#endif

/* === U S I N G =========================================================== */

using namespace std;
using namespace etl;
using namespace synfig;
using namespace studio;

/* === M A C R O S ========================================================= */

/* === G L O B A L S ======================================================= */

/* === P R O C E D U R E S ================================================= */

/* === M E T H O D S ======================================================= */

Widget_BoneChooser::Widget_BoneChooser()
{
}

Widget_BoneChooser::~Widget_BoneChooser()
{
}

void
Widget_BoneChooser::set_parent_canvas(synfig::Canvas::Handle x)
{
	assert(x);
	parent_canvas=x;
}

void
Widget_BoneChooser::set_value_(synfig::ValueNode_Bone::Handle data)
{
	set_value(data);
	activate();
}

void
Widget_BoneChooser::set_value(synfig::ValueNode_Bone::Handle data)
{
	set<ValueNode_Bone::Handle> affected_bones; // which bones are we currently editing the parent of - it can be more than one due to linking

	assert(parent_canvas);
	bone=data;

	// the ValueNode is either a ValueNode_Const or a ValueNode_Animated I think
	if (get_value_desc().is_value_node())
		affected_bones = ValueNode_Bone::get_affected_bones(get_value_desc().get_value_node());

	bone_menu=manage(new class Gtk::Menu());

	synfig::ValueNode_Bone::BoneMap::const_iterator iter;
	String label;
	Time time(parent_canvas->get_time());

	for(iter=synfig::ValueNode_Bone::map_begin(); iter!=synfig::ValueNode_Bone::map_end(); iter++)
	{
		GUID guid(iter->first);
		ValueNode_Bone::Handle bone_value_node(iter->second);

		if (affected_bones.count(bone_value_node.get()))
			continue;

		ValueNode::Handle parent(bone_value_node->get_link("parent"));
		set<ValueNode_Bone::Handle> parents(ValueNode_Bone::get_bones(parent));
		set<ValueNode_Bone::Handle>::iterator iter;
		for (iter = parents.begin(); iter != parents.end(); iter++)
			if (affected_bones.count(iter->get()))
				break;
		if (iter != parents.end())
			continue;

		label=(*(bone_value_node->get_link("name")))(time).get(String());
		if (label.empty()) label=guid.get_string();

		bone_menu->items().push_back(
			Gtk::Menu_Helpers::MenuElem(label,
										sigc::bind(
											sigc::mem_fun(
												*this,
												&Widget_BoneChooser::set_value_),
											bone_value_node)));
	}

	bone_menu->items().push_back(
		Gtk::Menu_Helpers::MenuElem(_("<None>"),
										sigc::bind(
											sigc::mem_fun(
												*this,
												&Widget_BoneChooser::set_value_),
											ValueNode_Bone::Handle())));

	set_menu(*bone_menu);

	if(bone)
		set_history(0);
}

const etl::handle<synfig::ValueNode_Bone> &
Widget_BoneChooser::get_value()
{
	return bone;
}
