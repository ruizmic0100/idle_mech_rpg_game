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
	
	std::string slot_str = "";
	if (slot == EquipmentSlot::HEAD) slot_str = "HEAD";
	if (slot == EquipmentSlot::CHEST) slot_str = "CHEST";
	if (slot == EquipmentSlot::ARMS) slot_str = "ARMS";
	if (slot == EquipmentSlot::LEGS) slot_str = "LEGS";
	if (slot == EquipmentSlot::GENERATOR) slot_str = "GENERATOR";
	if (slot == EquipmentSlot::LEFT_ARM_WEAPON) slot_str = "LEFT_ARM_WEAPON";
	if (slot == EquipmentSlot::RIGHT_ARM_WEAPON) slot_str = "RIGHT_ARM_WEAPON";
	if (slot == EquipmentSlot::LEFT_SHOULDER_WEAPON) slot_str = "LEFT_SHOULDER_WEAPON";
	if (slot == EquipmentSlot::RIGHT_SHOULDER_WEAPON) slot_str = "RIGHT_SHOULDER_WEAPON";
	std::cout << "Equipped item " << equipped_items[slot] << " on " << slot_str << " slot." << std::endl;
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
