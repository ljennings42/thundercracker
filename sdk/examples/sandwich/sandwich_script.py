import lxml.etree
import os
import os.path
import re
import tmx
import misc

EXP_REGULAR = re.compile(r"^([a-z]+)$")
EXP_PLUS = re.compile(r"^([a-z]+)\+(\d+)$")
EXP_MINUS = re.compile(r"^([a-z]+)\-(\d+)$")

class GameScript:
	def __init__(self, world, path):
		self.world = world
		self.path = path
		xml = lxml.etree.parse(path)
		self.quests = [Quest(i, elem) for i,elem in enumerate(xml.findall("quest"))]
		if len(self.quests) > 1:
			for index,quest in enumerate(self.quests[0:-1]):
				for otherQuest in self.quests[index+1:]:
					if quest.id == otherQuest.id:
						raise Exception("Duplicate Quest ID")
		self.quest_dict = dict((quest.id, quest) for quest in self.quests)
		self.unlockables = [ Flag(i, elem) for i,elem in enumerate(xml.findall("unlockables/flag")) ]
		self.unlockables_dict = dict((flag.id, flag) for flag in self.unlockables)
	
	def getquest(self, name):
		m = EXP_REGULAR.match(name)
		if m is not None:
			return self.quest_dict[name]
		m = EXP_PLUS.match(name)
		if m is not None:
			return self.quests[self.quest_dict[m.group(1)].index + int(m.group(2))]
		m = EXP_MINUS.match(name)
		if m is not None:
			return self.quests[self.quest_dict[m.group(1)].index - int(m.group(2))]
		raise Exception("Invalid Quest Identifier: " + name)

	def add_flag_if_undefined(self, id):
		if not id in self.unlockables_dict:
			flag = Flag(len(self.flags), id)
			self.unlockables.append(flag)
			self.unlockables_dict[id] = flag
		return self.unlockables_dict[id]


class Quest:
	def __init__(self, index, xml):
		self.index = index
		self.id = xml.get("id").lower()
		self.map = xml.get("map").lower()
		self.x = int(xml.get("x"))
		self.y = int(xml.get("y"))
		self.flags = [Flag(i, elem) for i,elem in enumerate(xml.findall("flag"))]
		if len(self.flags) > 32:
			raise Exception("Too Many Flags for Quest: %s" % self.id)
		if len(self.flags) > 1:
			for index,flag in enumerate(self.flags[0:-1]):
				for otherFlag in self.flags[index+1:]:
					if flag.id == otherFlag.id:
						raise Exception("Duplicate Quest Flag ID")
		self.flag_dict = dict((flag.id,flag) for flag in self.flags)
	
	def add_flag_if_undefined(self, id):
		if not id in self.flag_dict:
			flag = Flag(len(self.flags), id)
			self.flags.append(flag)
			self.flag_dict[id] = flag
		return self.flag_dict[id]


class Flag:
	def __init__(self, index, xml_or_id):
		self.index = index
		self.id = (xml_or_id if isinstance(xml_or_id, str) else xml_or_id.get("id")).lower()