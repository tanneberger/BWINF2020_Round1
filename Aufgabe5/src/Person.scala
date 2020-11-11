import scala.util.Random

case class Person(nr: Int, choice: Choice) {
  def this(presents: Int) = this(Random.nextInt(), new Choice(presents))

  def descGerman(got: Int): String =
    "Wichtel Nr. " + (nr+1).toHexString + " will Geschenk #" + (choice.fst+1) + " am liebsten, #" + (choice.snd+1) + " geht auch, #" + (choice.thd+1) + " wenns sein muss. Er bekommt Geschenk #" + (1+got) + ". Er ist\t\t" + (got match {
      case got if got == choice.fst => "glücklich."
      case got if got == choice.snd => "zufrieden."
      case got if got == choice.thd => "knapp dran."
      case _ => "unzufrieden."
    })

  def fight(other: Person, nr: Int, free: scala.collection.Set[Int]): Person = {
    if(nr >= 2 || other == this) // quit the fight
      return this
    val s = free.contains(choice(nr+1))
    val o = free.contains(other.choice(nr+1))

    (s, o) match {
      case (true, false) => this
      case (false, true) => other
      case (false, false) => fight(other, nr +1, free)
      case (true, true) => this // just someone, no reason to fight
    }
  }

  def ==(other: Person): Boolean = nr == other.nr
}

