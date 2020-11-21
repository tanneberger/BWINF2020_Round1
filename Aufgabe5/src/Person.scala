import scala.util.Random

case class Person(nr: Int, choice: Choice) {
  def this(presents: Int) = this(Random.nextInt(), new Choice(presents))

  def descGerman(got: Int): String =
    "Wichtel '" + (nr+1).toHexString + "' will Geschenk #" + (choice.fst+1) + " am liebsten, #" + (choice.snd+1) + " geht auch, #" + (choice.thd+1) + " wenns sein muss. Er bekommt Geschenk #" + (1+got) + ". Er ist\t\t" + (happiness(got) match {
      case 3 => "glücklich."
      case 2 => "zufrieden."
      case 1 => "knapp dran."
      case 0 => "unzufrieden."
    })

  def happiness(present: Int): Int = present match {
    case got if got == choice.fst => 3
    case got if got == choice.snd => 2
    case got if got == choice.thd => 1
    case _ => 0
  }

  def fight(other: Person, nr: Int, free: scala.collection.Set[Int]): Person = {
    if(nr >= 2 || other == this) // quit the fight
      return this
    val s = free.contains(choice(nr+1))
    val o = free.contains(other.choice(nr+1))

    (s, o) match {
      case (true, false) => other // the other one's taken, so he wins
      case (false, true) => this // ours is taken, we win the fight
      case (false, false) | (true, true) => fight(other, nr +1, free)
    }
  }

  def ==(other: Person): Boolean = nr == other.nr
}

