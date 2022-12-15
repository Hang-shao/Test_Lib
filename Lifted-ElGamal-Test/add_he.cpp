#include <mcl/fp.hpp>
#include <mcl/ecparam.hpp>
#include <mcl/elgamal.hpp>

struct TagZn;
typedef mcl::FpT<> Fp;
typedef mcl::FpT<TagZn> Zn;
typedef mcl::EcT<Fp> Ec;
typedef mcl::ElgamalT<Ec, Zn> ElgamalEc;

const mcl::EcParam& para = mcl::ecparam::secp192k1;
cybozu::RandomGenerator g_rg;

int main()
{
	Ec P;
	mcl::initCurve<Ec, Zn>(para.curveType, &P);
	const size_t bitSize = Zn::getBitSize();
	/*
		Zn = <P>
	*/
	ElgamalEc::PrivateKey prv;
	prv.init(P, bitSize, g_rg);
	prv.setCache(0, 60000);
	const ElgamalEc::PublicKey& pub = prv.getPublicKey();

	//加解密测试
	const int m1 = 12345;
	const int m2 = 17655;
	ElgamalEc::CipherText c1, c2;
	pub.enc(c1, m1, g_rg);
	pub.enc(c2, m2, g_rg);
	std::cout<<"c1="<<c1.getStr()<<std::endl;
	std::cout<<"c2="<<c2.getStr()<<std::endl;
	std::cout<<"dec1="<<prv.dec(c1)<<std::endl;
	std::cout<<"dec2="<<prv.dec(c2)<<std::endl;
	std::cout<<"加解密测试：ok!"<<std::endl;

	//enc(m1) enc(m2) = enc(m1 + m2)
	c1.add(c2);
	std::cout<<"m1+m2="<<prv.dec(c1)<<std::endl;
	std::cout<<"同态加法（c2c）测试：ok!"<<std::endl;

	// enc(m1) x = enc(m1 + x)
	const int x = 2;
	pub.add(c2, x);
	std::cout<<"m2+x="<<prv.dec(c2)<<std::endl;
	std::cout<<"同态加法(c2m)测试：ok!"<<std::endl;

	// check mul
	Zn dec2;	
	c2.mul(x);
	prv.dec(dec2, c2);
	std::cout<<"m2*x="<<dec2<<std::endl;
	std::cout<<"同态乘法(c2m)测试：ok!"<<std::endl;
	return 0;
}