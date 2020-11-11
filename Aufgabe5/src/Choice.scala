import scala.util.Random

case class Choice(fst: Int, snd: Int, thd: Int) {
  def this(presents: Int) = this(Random.nextInt(presents), Random.nextInt(presents), Random.nextInt(presents))
  def apply(at: Int): Int = at match {
    case 0 => fst
    case 1 => snd
    case 2 => thd
    case 3 => throw new IndexOutOfBoundsException(at)
  }
}
