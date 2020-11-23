package de.vincenteichhorn;

import java.io.*;
import java.util.Arrays;
import java.util.Scanner;

public class Main {

    //Vergleichsschema für einen Baulwurf-Bau
    static boolean[][] schema = new boolean[][] {
            {true, true, true},
            {true, false, true},
            {true, false, true},
            {true, true, true},
    };

    public static void main(String[] args) throws IOException {
        BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));

        String row1 = reader.readLine();
        int width = Integer.valueOf(row1);
        String row2 = reader.readLine();
        int height = Integer.valueOf(row2);

        //Leere Karte erstellen
        boolean[][] map = newMap(width, height);

        //Eingabe in Karte übertragen
        for(int i = 0; i < height; i++) {
            String rown = reader.readLine();
            for(int j = 0; j < width; j++) {
                if(rown.charAt(j) == 'X') map[i][j] = true;
            }
        }

        //Anzahl Baulwürfe für die Karte Berechnen und Ausgeben
        System.out.println("Auf der Karte sind " + countBaulwürfe(map) + " zu finden.");

    }

    /**
     * Gibt eine leere Karte mit gegebenen Abmessungen zurück
     * @param width - Breite der Karte
     * @param height - Höhe der Karte
     * @return Karte aus zweidimensionales boolean array. false ist ein leere Feld, true ein markiertes
     */
    public static boolean[][] newMap(int width, int height) {
        //Neue null Karte
        boolean[][] map = new boolean[height][width];
        //Karte mit false füllen
        for(int i = 0; i < height; i++) {
            for(int j = 0; j < width; j++) {
                map[i][j] = false;
            }
        }
        return map;
    }

    /**
     * Zählt die Baulwürfe auf einer gegebenen Karte
     * @param map - Karte aus 2d boolean array
     * @return Anzahl der Baulwürfe als int
     */
    public static int countBaulwürfe(boolean[][] map) {
        int n = 0;
        //Iterativ auf Bauwurf-Struktur prüfen
        for(int i = 0; i < map.length; i++) {
            for(int j = 0; j < map[0].length; j++) {
                if(compare(getPart(map, j, i), schema)) {
                    n++;
                }
            }
        }
        return n;
    }

    /**
     * Vergleicht eine Karte mti einem gegeben Vergleichsschema
     * @param part - Karte
     * @param schema - Vergleichsschema
     * @return Übereinstimmung true
     */
    public static boolean compare(boolean[][] part, boolean[][] schema) {
        //Wenn Größe von part und schema nicht passt: abbrechen return false
        if(part.length != schema.length || part[0].length != schema[0].length) return false;

        //Vergeleichen von part und schema
        for(int i = 0; i < part.length; i++) {
            for(int j = 0; j < part[0].length; j++) {
                //Wenn Unstimmigkeit gefunden: abbrechen return false
                if(part[i][j] != schema[i][j]) return false;
            }
        }

        //part und schema stimmen überein
        return true;
    }

    /**
     * Schneidet einen 3x4 oder kleineren Teil aus einer Karte an angegebener Punktkoordinate raus.
     * @param map - Urpsungskarte
     * @param x - Spaltenindex des oberen linken Feldes
     * @param y - Reihenindex des oberen linken Feldes
     * @return Karte mit den Abmessungen 3x4 oder kleiner
     */
    public static boolean[][] getPart(boolean[][] map, int x, int y) {
        //Endkoordinate für Reihe berechnen
        int endY = y + 4;
        if(endY >= map.length ) endY = map.length;
        //Alle Reihen in berech y:endY heraustrennen
        boolean[][] part = Arrays.copyOfRange(map, y, endY);
        //Endkoordinate für Spalte berechnen
        int endX = x + 3;
        if(endX >= map[0].length) endX = map[0].length;
        //Jeder Reihe nicht genutze Spalten wegschneiden
        for(int i = 0; i < part.length; i++) {
            part[i] = Arrays.copyOfRange(part[i], x, endX);
        }
        return part;
    }
}
