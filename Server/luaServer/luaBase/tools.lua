module(..., package.seeall)

function transprotos(pba, pbb)
	pba:ParseFromString(pbb:SerializeToString())
end
