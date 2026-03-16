#!/bin/bash
# Dragon Tear Hoard Manager
# Copyright (C) 2024
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

# Steam Deck Deployment Script Template
# Usage: ./deck_deploy.sh <game> <steam_deck_ip> <path_to_file>

if [ "$#" -ne 3 ]; then
    echo "Usage: $0 <game> <steam_deck_ip> <path_to_file>"
    echo "  <game> must be 'nms' or 'cyberpunk'"
    exit 1
fi

GAME=$1
STEAM_DECK_IP=$2
FILE_PATH=$3
STEAM_DECK_USER="deck"

if [ "$GAME" == "nms" ]; then
    TARGET_DIR="/home/deck/.local/share/Steam/steamapps/common/No Man's Sky/GAMEDATA/PCBANKS/MODS/"
elif [ "$GAME" == "cyberpunk" ]; then
    TARGET_DIR="/home/deck/.local/share/Steam/steamapps/common/Cyberpunk 2077/bin/x64/plugins/cyber_engine_tweaks/mods/"
else
    echo "Error: game must be 'nms' or 'cyberpunk'"
    exit 1
fi

if [ ! -e "$FILE_PATH" ]; then
    echo "Error: file not found at $FILE_PATH"
    exit 1
fi

echo "Deploying $FILE_PATH for $GAME to Steam Deck at $STEAM_DECK_IP..."
rsync -avzP "$FILE_PATH" "${STEAM_DECK_USER}@${STEAM_DECK_IP}:'${TARGET_DIR}'"

if [ $? -eq 0 ]; then
    echo "Deployment successful."
else
    echo "Deployment failed."
fi
