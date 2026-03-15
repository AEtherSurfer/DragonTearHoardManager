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
# Usage: ./deck_deploy.sh <steam_deck_ip> <path_to_pak_file>

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <steam_deck_ip> <path_to_pak_file>"
    exit 1
fi

STEAM_DECK_IP=$1
PAK_FILE_PATH=$2
STEAM_DECK_USER="deck"
NMS_MODS_DIR="/home/deck/.local/share/Steam/steamapps/common/No Man's Sky/GAMEDATA/PCBANKS/MODS/"

if [ ! -f "$PAK_FILE_PATH" ]; then
    echo "Error: .pak file not found at $PAK_FILE_PATH"
    exit 1
fi

echo "Deploying $PAK_FILE_PATH to Steam Deck at $STEAM_DECK_IP..."
rsync -avzP "$PAK_FILE_PATH" "${STEAM_DECK_USER}@${STEAM_DECK_IP}:'${NMS_MODS_DIR}'"

if [ $? -eq 0 ]; then
    echo "Deployment successful."
else
    echo "Deployment failed."
fi
