#!/usr/bin/python3

"""BWINF Aufgabe 3"""

__version__ = "0.1"
__author__ = "Eric Wolf"

from json import load as json_load
from abc import ABC, abstractmethod
from random import random
from typing import NamedTuple, Iterable, Iterator, Tuple


class Player(NamedTuple):
    """Repräsentation eines Spielers"""

    strength: int

    def match(self, opponent):
        """gebe Gewinner einer Runde zurück"""
        marbles = self.strength + opponent.strength
        if random() < self.strength / marbles:
            return self
        else:
            return opponent


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

    def matches(self) -> Iterator[Tuple[int, Player, int, Player]]:
        """Iterator über alle einzigartigen Begegnungen"""
        for pnumber, player in enumerate(self.players, start=1):
            for onumber, opponent in enumerate(self.players[pnumber:], start=pnumber + 1):
                yield pnumber, player, onumber, opponent


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
        wins0, wins1 = 0, 0
        for _ in range(self.rounds):
            if player0.match(player1) is player0:
                wins0 += 1
            else:
                wins1 += 1
        if wins0 > wins1:
            return player_index0
        elif wins0 < wins1:
            return player_index1
        else:                                           # Gleichstand
            return min(player_index0, player_index1)    # kleinste Nummer gewinnt, wenn beide Spieler die gleiche Nummer haben ist es egal welchen wir auswählen


def parse_node(json) -> TournamentNode:
    """erstelle ein Turnier aus einer geparsten JSON Datei"""
    if json["type"] == "Match":
        return MatchNode(
            json["rounds"],
            parse_node(json["player0"]),
            parse_node(json["player1"])
        )
    elif json["type"] == "Player":
        return PlayerNode(json["number"])
    else:
        raise ValueError("Unbekannter Node")


if __name__ == "__main__":
    from argparse import ArgumentParser

    parser = ArgumentParser(description="BWINF Aufgabe 3")
    parser.add_argument("players", help="Pfad zu einer Spielerdatei", type=str)
    parser.add_argument("repetitions", help="Anzahl der Wiederholungen jedes Turniers", type=int)
    parser.add_argument("plan", help="Pfad(e) zu einem Turnierplan", type=str, nargs="*")
    parser.add_argument("-v", "--version", help="zeige Versionsnummer", action="version", version=__version__)
    args = parser.parse_args()

    with open(args.players, "r") as fd:
        length = int(fd.readline().rstrip())
        players = Players([Player(int(line.rstrip())) for line in fd])
        if length != len(players):
            raise ValueError("Spieleranzahl stimmt nicht überein")

    for path in args.plan:
        with open(path, "r") as fd:
            tournament = parse_node(json_load(fd))
        wins = 0
        for _ in range(args.repetitions):
            if tournament.simulate(players) == players.strongest:
                wins += 1
        print(f"{path}: Spieler {players.strongest} siegte {wins} mal in {args.repetitions} Turnieren ({wins / args.repetitions * 100}%)")
