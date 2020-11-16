#!/usr/bin/python3

"""BWINF Aufgabe 3"""

__version__ = "0.1"
__author__ = "Eric Wolf"

import json
from abc import ABC, abstractmethod
from random import random
from argparse import ArgumentParser
from typing import NamedTuple, Iterable, Iterator


class Player(NamedTuple):
    """Repräsentation eines Spielers"""

    strength: int


class Players:
    """Menge aller Spieler"""
    def __init__(self, iterable: Iterable[Player] = ()):
        self.players = tuple(iterable)
        self.strongest = self.get_strongest()

    def __repr__(self) -> str:
        return repr(self.players)

    def __len__(self) -> int:
        return len(self.players)

    def __contains__(self, obj) -> bool:
        return obj in self.players

    def __getitem__(self, index: int) -> Player:
        if index > 0:
            return self.players[index - 1]
        raise IndexError("Spielernummer kleiner als 1")

    def __iter__(self) -> Iterator[Player]:
        return iter(self.players)

    def get_strongest(self) -> int:
        strongest = 0
        count = 0
        for index, player in enumerate(self.players, start=1):
            if strongest == 0 or self[strongest] < player:
                strongest = index
                count = 1               # setze Anzahl zurück wenn neuer Spieler gefunden wurde
            elif self[strongest] == player:
                count += 1
            else:
                pass
        if count == 1:
            return strongest    # Initialwert wird nie zurückgegeben bei count == 1
        raise ValueError("es gibt keinen stärksten Spieler")


class TournamentNode(ABC):
    """Repräsentation eines Turniers"""
    @abstractmethod
    def simulate(self, players: Players) -> int:
        """simuliere ein Turnier und gib den Gewinner zurück"""
        raise NotImplementedError


class PlayerNode(TournamentNode):
    """Repräsentation eines Spielers im Turnierplan"""
    def __init__(self, number: int) -> None:
        self.number = number

    def simulate(self, players: Players) -> int:
        """Turnier besteht nur aus einem Spieler, er gewinnt"""
        return self.number


class MatchNode(TournamentNode):
    """Repräsentation eines Zusammentreffens im Turnierplan"""
    def __init__(self, rounds: int, player0: TournamentNode, player1: TournamentNode) -> None:
        self.rounds = rounds
        self.player0 = player0
        self.player1 = player1

    def simulate(self, players: Players) -> int:
        player_index0 = self.player0.simulate(players)
        player_index1 = self.player1.simulate(players)
        player0 = players[player_index0]
        player1 = players[player_index1]
        marbles = player0.strength + player1.strength
        wins0, wins1 = 0, 0
        for _ in range(self.rounds):
            if random() < player0.strength / marbles:
                wins0 += 1
            else:
                wins1 += 1
        if wins0 > wins1:
            return player_index0
        elif wins0 < wins1:
            return player_index1
        else:               # Gleichstand
            return min(player_index0, player_index1)    # kleinste Nummer gewinnt, wenn beide Spieler die gleiche Nummer haben ist es egal welchen wir auswählen


parser = ArgumentParser(description="BWINF Aufgabe 3")
parser.add_argument("players", help="Pfad zu einer Spielerdatei", type=str)
parser.add_argument("repetitions", help="Anzahl der Wiederholungen jedes Turniers", type=int)
parser.add_argument("plan", help="Pfad zu einem Turnierplan", type=str, nargs="*")
parser.add_argument("-v", "--version", help="zeige Versionsnummer", action="version", version=__version__)
args = parser.parse_args()

with open(args.players, "r") as fd:
    players = Players([Player(int(line.rstrip())) for line in fd])

print(players, players.strongest)   # DEBUG

match = MatchNode(  # TODO: JSON Parser
    5,
    MatchNode(
        5,
        PlayerNode(1),
        PlayerNode(2)
    ),
    MatchNode(
        5,
        PlayerNode(3),
        PlayerNode(4)
    )
)

print(match.simulate(players))  # DEBUG
