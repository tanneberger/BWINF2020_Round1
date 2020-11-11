import scala.collection.mutable

object Main {
  def main(args: Array[String]): Unit = {
    println("--- BWINF AUFGABE 5 ---")
    println("Bitte geben sie eine URL zu einem Wichtel-Layout oder eins direkt hier ein.")
    println("(Geben sie <no>, <nein>, <n> oder so ein zum Beenden)")
    print("> ")

    while(true) {
      val userinput = scala.io.StdIn.readLine()
      val text = try {
        new String(new java.net.URL(userinput).openStream().readAllBytes())
      } catch {
        case _: Exception => userinput
      }

      if(text.toLowerCase.contains('n')) {
        println("Vielen Dank fürs Ausprobieren, einen schönen Tag noch!")
        return
      }

      val persons: Set[Person] = parse(text).getOrElse({
        println("Das erhaltene Wichtel-Layout ist invalid. Bitte beheben sie den Fehler!")
        Set()
      })

      for((present, person) <- solve(persons).toIndexedSeq.sortBy(_._2.nr)) {
        println(person.descGerman(present))
      }
      print("Weiter > ")
    }
  }

  def parse(prog: String): Option[Set[Person]] = {
    try {
      val ints = prog.split("\\s+").iterator
      val len = ints.next.toInt
      val persons = Set.newBuilder[Person]

      for(i <- 0 until len) {
        persons.addOne(Person(i, Choice(ints.next.toInt-1, ints.next.toInt-1, ints.next.toInt-1)))
      }

      Some(persons.result)
    } catch {
      case _: Exception => None
    }
  }

  def solve(persons: Set[Person]): mutable.Map[Int, Person] = {
    val taken = mutable.HashMap[Int, Person]()
    val queue = mutable.HashSet.from(persons)
    val free = mutable.HashSet.from(0 until persons.size)

    for(i <- 0 until 3) {
      // add all person to the contested list so each one gets at least one chance to wish
      val contested = mutable.HashMap.from(queue.map(p => (p.choice(i), p)))
      contested --= taken.keys
      free --= contested.keys

      // reorder the contested list
      for(person <- queue) {
        val wish = person.choice(i)

        if(contested.contains(wish)) {
          val winner = person.fight(contested(wish), i, free)
          contested.put(wish, winner)
        }
      }

      queue --= contested.values
      taken ++= contested
    }

    taken.addAll(free zip queue)
    taken
  }

}