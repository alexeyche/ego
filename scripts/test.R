
require(Rdnn)
rr = function(l, n=0) {
    return(as.matrix(read.csv(sprintf("/var/tmp/%s-%s.csv", l, n), header=F)))
}
Ks = rr("Ks")
ms = rr("ms")
kss = rr("kss")
Alpha = rr("Alpha")
Fs2 = rr("Fs2")
Linv = rr("Linv")
DiagW = rr("DiagW")
Fmu = rr("Fmu")
L = rr("L")