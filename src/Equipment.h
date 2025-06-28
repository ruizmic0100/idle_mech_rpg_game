#ifndef EQUIPMENT_H
#define EQUIPMENT_H

#include <iostream>
#include <map>
#include <memory>
#include "Item.h"
#include "Stats.h"

class Equipment {
public:
	std::shared_ptr<Item> equip(std::shared_ptr<Item> new_item); // Returns previously equipped item, if any
	std::shared_ptr<Item> unequip(EquipmentSlot slot);
	std::shared_ptr<const Item> getItem(EquipmentSlot slot) const;
	Stats getTotalStats() const;
	const std::map<EquipmentSlot, std::shared_ptr<Item>>& getEquippedItems() const;

private:
	std::map<EquipmentSlot, std::shared_ptr<Item>> equipped_items;
};

#endif // EQUIPMENT_H
