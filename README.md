<!--
Dragon Tear Hoard Manager
Copyright (C) 2024

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
-->

# Dragon Tear Hoard Manager

Welcome to the Dragon Tear Hoard Manager, an inventory optimization mod suite!

## Project Structure

This project is a monorepo that supports multiple games using a shared C++ core:

*   `/core/`: Game-agnostic C++ core logic (e.g., `TearEngine.hpp`).
*   `/games/nms/`: No Man's Sky specific mod files, mappings, and hooks.
*   `/games/cyberpunk-2077/`: Cyberpunk 2077 specific files, including RED4ext C++ plugin, Redscript, and Lua.
*   `/tools/`: Shared deployment and utility scripts.
*   `/tests/`: Shared test suite for the core engine.

## Hoard Modes

The "Tear Engine" provides logic to handle different types of game inventories:

1.  **Slot-Limited (NMS)**: Focuses on stack sizes and slot availability. Items that exceed configured thresholds are marked for action based on current inventory space.
2.  **Mass-Limited (Cyberpunk)**: Focuses on value-to-weight ratios and encumbrance. The engine calculates an efficiency ratio and factors in the player's current carrying weight vs. max carrying weight to dynamically adjust discard thresholds.

## Dragon Psychology 101

The core of this inventory manager lies in "Dragon Psychology," modeled after the mythical instincts of a dragon hoarding its treasure. The "Tear Engine" uses a unique formula—the "Dragon Scale" logic—to determine the optimal action for any given item:

*   **KEEP**: The Dragon's Eye stays open! High rarity, market value, and utility make this an item worth guarding fiercely.
*   **SELL**: Shed a GOLDEN TEAR. Low utility or value items are cast away, leaving only room for true treasure.
*   **USE**: The intermediate state. It serves an immediate purpose without necessarily needing to be hoarded or cast away.

### "The Mercenary" Archetype (Cyberpunk 2077)

With our expansion to Cyberpunk 2077, we introduce "The Mercenary" logic. In Night City, carrying capacity is vital, and the choice is often between pure eddies (selling) and crafting components (disassembling). The engine calculates an **Efficiency Ratio** (Value per Weight unit).
*   High efficiency items are marked to **SELL** for Eddies.
*   Low efficiency items are marked to **USE** (Disassemble) for valuable crafting components.

## License & AGPL-3.0 Requirements

This project is licensed under the **GNU Affero General Public License v3.0 (AGPL-3.0)**.

### What this means for you:
*   You are free to use, modify, and distribute this software.
*   If you modify this software and make it available over a network (even as a backend service), you **must** provide the complete corresponding source code to your users.
*   All derived works must also be licensed under the AGPL-3.0.

For more details, please see the `LICENSE` file included in this repository.
