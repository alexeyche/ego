
require(Ryacas)

Deriv = function(s, v, simplify=TRUE) {
    o = sprintf("D(%s) %s", v, s)
    if(simplify) {
        o = sprintf("Simplify(Simplify(%s))", o)
    }
    return(o)
}
cats = function(s, ...) {
    cat(sprintf(s, ...), "\n")
}

yacas("Write(\"Hello world\")")

funlist = list(
    list(
        "SqExp"
      , "Exp( -(r^2) / 2 )"
      , list("r")
    )
  , list(
        "Matern3/2"
      , "(1 + Sqrt(3)*r) * Exp(-Sqrt(3)*r)"
      , list("r")
    )
  , list(
        "Matern5/2"
      , "(1 + Sqrt(5)*r + 5*(r^2)/3 * Exp(-Sqrt(5)*r)"
      , list("r")
    )
  , list(
        "GammaExp"
      , "Exp(-r^g)"
      , list("r", "g")
    )
  , list(
        "RatQuad"
      , "(1 + (r^2)/(2*alpha))^(-alpha)"
      , list("r", "alpha")
  )
)

for(fdesc in funlist) {
    fname = fdesc[[1]]
    f = fdesc[[2]]
    vars = fdesc[[3]]
    cats("%s function\n\tForm:\n%s", fname, f)
    for(var in vars) {
        cats("\tDerivatives dk/d%s", var)
        print(yacas(Deriv(f, var)))
    }
    cat("\n")
}