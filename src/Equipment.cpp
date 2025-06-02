#include "Equipment.h"
#include "Item.h"

std::shared_ptr<Item> Equipment::equip(std::shared_ptr<Item> new_item) {
	if (!new_item) return nullptr; // Cannot equip a null item

	EquipmentSlot slot = new_item->getSlot();
	if (slot == EquipmentSlot::NONE) return new_item; // Cannot equip item with no slot (return it back)

	std::shared_ptr<Item> old_item = nullptr;
	auto it = equipped_items.find(slot);
	if (it != equipped_items.end()) {
		old_item != it->second; // Store the old item
	}
	equipped_items[slot] = new_item; // Equip the new item
	return old_item; // Return the previously equipped item
}

std::shared_ptr<Item> Equipment::unequip(EquipmentSlot slot) {
	std::shared_ptr<Item> unequipped_item = nullptr;

	auto it = equipped_items.find(slot);
	if (it != equipped_items.end()) {
		unequipped_item = it->second;
		equipped_items.erase(it);
	}
	return unequipped_item;
}

std::shared_ptr<const Item> Equipment::getItem(EquipmentSlot slot) const {
	auto it = equipped_items.find(slot);
	if (it != equipped_items.end()) {
		return it->second;
	}

	return nullptr;
}

const std::map<EquipmentSlot, std::shared_ptr<Item>>& Equipment::getEquippedItems() const {
	return equipped_items;
}

Stats Equipment::getTotalStats() const {
	Stats total_equipment_stats;
	for (const auto& slot_item_pair : equipped_items) {
		const std::shared_ptr<Item>& item = slot_item_pair.second;
		if (item) { // Check if an item is actually equipped in this slot
			const Stats& item_instance_stats = item->getStats(); // This should be the item's instance_stats
			for (const auto& stat_pair : item_instance_stats) {
				addStat(total_equipment_stats, stat_pair.first, stat_pair.second);
			}
		}
	}

	return total_equipment_stats;
}
