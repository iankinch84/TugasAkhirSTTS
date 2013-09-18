using System;
using System.Text;
using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Diagnostics;
using BloomFilterLib;

namespace TestProject
{
	/// <summary>
	/// Tests for BloomFilter
	/// </summary>
	[TestClass]
	public class BloomFilterTest
	{
		protected readonly Random r = new Random((int)(DateTime.Now.Ticks % int.MaxValue));

		public BloomFilterTest() {}

		private TestContext testContextInstance;

		/// <summary>
		///Obtient ou définit le contexte de test qui fournit
		///des informations sur la série de tests active ainsi que ses fonctionnalités.
		///</summary>
		public TestContext TestContext
		{
			get
			{
				return testContextInstance;
			}
			set
			{
				testContextInstance = value;
			}
		}

		#region Attributs de tests supplémentaires
		//
		// Vous pouvez utiliser les attributs supplémentaires suivants lorsque vous écrivez vos tests :
		//
		// Utilisez ClassInitialize pour exécuter du code avant d'exécuter le premier test de la classe
		// [ClassInitialize()]
		// public static void MyClassInitialize(TestContext testContext) { }
		//
		// Utilisez ClassCleanup pour exécuter du code une fois que tous les tests d'une classe ont été exécutés
		// [ClassCleanup()]
		// public static void MyClassCleanup() { }
		//
		// Utilisez TestInitialize pour exécuter du code avant d'exécuter chaque test 
		// [TestInitialize()]
		// public void MyTestInitialize() { }
		//
		// Utilisez TestCleanup pour exécuter du code après que chaque test a été exécuté
		// [TestCleanup()]
		// public void MyTestCleanup() { }
		//
		#endregion

		[TestMethod]
		public void testConstructorCNK()
		{
			Trace.WriteLine("BloomFilter(c,n,k)");

			for (int i = 0 ; i < 10000 ; i++) {
				double c = r.Next(20) + 1;
				int n = r.Next(10000) + 1;
				int k = r.Next(20) + 1;
				BloomFilter bf = new BloomFilter(c, n, k);
				Assert.AreEqual(bf.getK(), k);
				Assert.AreEqual(bf.getExpectedBitsPerElement(), c, 0);
				Assert.AreEqual(bf.getExpectedNumberOfElements(), n);
				Assert.AreEqual(bf.size(), c*n, 0);
			}
		}


		/**
		 * Test of createHash method, of class BloomFilter.
		 * @
		 */
		[TestMethod]
		public void testCreateHash_String()
		{
			Trace.WriteLine("createHash");
			string val = Guid.NewGuid().ToString();
			int result1 = BloomFilter.createHash(val);
			int result2 = BloomFilter.createHash(val);
			Assert.AreEqual(result2, result1);
			int result3 = BloomFilter.createHash(Guid.NewGuid().ToString());

			Assert.AreNotSame(result3, result2);

			int result4 = BloomFilter.createHash(Encoding.UTF8.GetBytes(val));
			Assert.AreEqual(result4, result1);
		}

		/**
		 * Test of createHash method, of class BloomFilter.
		 * @
		 */
		[TestMethod]
		public void testCreateHash_byteArr()
		{
			Trace.WriteLine("createHash");
			string val = Guid.NewGuid().ToString();
			byte[] data = Encoding.UTF8.GetBytes(val);
			int result1 = BloomFilter.createHash(data);
			int result2 = BloomFilter.createHash(val);
			Assert.AreEqual(result1, result2);
		}

		/**
		 * Test of createHash method, of class BloomFilter.
		 * @
		 */
		[TestMethod]
		public void testCreateHashes_byteArr()
		{
			Trace.WriteLine("createHashes");
			string val = Guid.NewGuid().ToString();
			byte[] data = Encoding.UTF8.GetBytes(val);
			int[] result1 = BloomFilter.createHashes(data, 10);
			int[] result2 = BloomFilter.createHashes(data, 10);
			Assert.AreEqual(result1.Length, 10);
			Assert.AreEqual(result2.Length, 10);
			CollectionAssert.AreEqual(result1, result2);
			int[] result3 = BloomFilter.createHashes(data, 5);
			Assert.AreEqual(result3.Length, 5);
			for (int i = 0 ; i < result3.Length ; i++)
				Assert.AreEqual(result3[i], result1[i]);

		}

		/**
		 * Test of equals method, of class BloomFilter.
		 * @
		 */
		[TestMethod]
		public void testEquals()
		{
			Trace.WriteLine("equals");
			BloomFilter instance1 = new BloomFilter(1000, 100);
			BloomFilter instance2 = new BloomFilter(1000, 100);

			for (int i = 0 ; i < 100 ; i++) {
				string val = Guid.NewGuid().ToString();
				instance1.add(val);
				instance2.add(val);
			}

			Assert.IsTrue(instance1.Equals(instance2));
			Assert.IsTrue(instance2.Equals(instance1));

			instance1.add("Another entry"); // make instance1 and instance2 different before clearing

			instance1.clear();
			instance2.clear();

			Assert.IsTrue(instance1.Equals(instance2));
			Assert.IsTrue(instance2.Equals(instance1));

			for (int i = 0 ; i < 100 ; i++) {
				string val = Guid.NewGuid().ToString();
				instance1.add(val);
				instance2.add(val);
			}

			Assert.IsTrue(instance1.Equals(instance2));
			Assert.IsTrue(instance2.Equals(instance1));
		}

		/**
		 * Test of hashCode method, of class BloomFilter.
		 * @
		 */
		[TestMethod]
		public void testHashCode()
		{
			Trace.WriteLine("hashCode");

			BloomFilter instance1 = new BloomFilter(1000, 100);
			BloomFilter instance2 = new BloomFilter(1000, 100);

			Assert.IsTrue(instance1.GetHashCode() == instance2.GetHashCode());

			for (int i = 0 ; i < 100 ; i++) {
				string val = Guid.NewGuid().ToString();
				instance1.add(val);
				instance2.add(val);
			}

			Assert.IsTrue(instance1.GetHashCode() == instance2.GetHashCode());

			instance1.clear();
			instance2.clear();

			Assert.IsTrue(instance1.GetHashCode() == instance2.GetHashCode());

			instance1 = new BloomFilter(100, 10);
			instance2 = new BloomFilter(100, 9);
			Assert.IsFalse(instance1.GetHashCode() == instance2.GetHashCode());

			instance1 = new BloomFilter(100, 10);
			instance2 = new BloomFilter(99, 9);
			Assert.IsFalse(instance1.GetHashCode() == instance2.GetHashCode());

			instance1 = new BloomFilter(100, 10);
			instance2 = new BloomFilter(50, 10);
			Assert.IsFalse(instance1.GetHashCode() == instance2.GetHashCode());
		}

		/**
		 * Test of expectedFalsePositiveProbability method, of class BloomFilter.
		 */
		[TestMethod]
		public void testExpectedFalsePositiveProbability()
		{
			// These probabilities are taken from the bloom filter probability table at
			// http://pages.cs.wisc.edu/~cao/papers/summary-cache/node8.html
			Trace.WriteLine("expectedFalsePositiveProbability");
			BloomFilter instance = new BloomFilter(1000, 100);
			double expResult = 0.00819; // m/n=10, k=7
			double result = instance.expectedFalsePositiveProbability();
			Assert.AreEqual(instance.getK(), 7);
			Assert.AreEqual(expResult, result, 0.000009);

			instance = new BloomFilter(100, 10);
			expResult = 0.00819; // m/n=10, k=7
			result = instance.expectedFalsePositiveProbability();
			Assert.AreEqual(instance.getK(), 7);
			Assert.AreEqual(expResult, result, 0.000009);

			instance = new BloomFilter(20, 10);
			expResult = 0.393; // m/n=2, k=1
			result = instance.expectedFalsePositiveProbability();
			Assert.AreEqual(1, instance.getK());
			Assert.AreEqual(expResult, result, 0.0005);

			instance = new BloomFilter(110, 10);
			expResult = 0.00509; // m/n=11, k=8
			result = instance.expectedFalsePositiveProbability();
			Assert.AreEqual(8, instance.getK());
			Assert.AreEqual(expResult, result, 0.00001);
		}

		/**
		 * Test of clear method, of class BloomFilter.
		 */
		[TestMethod]
		public void testClear()
		{
			Trace.WriteLine("clear");
			BloomFilter instance = new BloomFilter(1000, 100);
			for (int i = 0 ; i < instance.size() ; i++)
				instance.setBit(i, true);
			instance.clear();
			for (int i = 0 ; i < instance.size() ; i++)
				Assert.AreEqual(instance.getBit(i), false);
		}

		/**
		 * Test of add method, of class BloomFilter.
		 * @
		 */
		[TestMethod]
		public void testAdd()
		{
			Trace.WriteLine("add");
			BloomFilter instance = new BloomFilter(1000, 100);

			for (int i = 0 ; i < 100 ; i++) {
				string val = Guid.NewGuid().ToString();
				instance.add(val);
				Assert.IsTrue(instance.contains(val));
			}
		}

		/**
		 * Test of addAll method, of class BloomFilter.
		 * @
		 */
		[TestMethod]
		public void testAddAll()
		{
			Trace.WriteLine("addAll");
			List<string> v = new List<string>();
			BloomFilter instance = new BloomFilter(1000, 100);

			for (int i = 0 ; i < 100 ; i++)
				v.Add(Guid.NewGuid().ToString());

			instance.addAll(v);

			for (int i = 0 ; i < 100 ; i++)
				Assert.IsTrue(instance.contains(v[i]));
		}

		/**
		 * Test of contains method, of class BloomFilter.
		 * @
		 */
		[TestMethod]
		public void testContains()
		{
			Trace.WriteLine("contains");
			BloomFilter instance = new BloomFilter(10000, 10);

			for (int i = 0 ; i < 10 ; i++) {
				instance.add(Convert.ToString(i, 2));
				Assert.IsTrue(instance.contains(Convert.ToString(i, 2)));
			}

			Assert.IsFalse(instance.contains(Guid.NewGuid().ToString()));
		}

		/**
		 * Test of containsAll method, of class BloomFilter.
		 * @
		 */
		[TestMethod]
		public void testContainsAll()
		{
			Trace.WriteLine("containsAll");
			List<string> v = new List<string>();
			BloomFilter instance = new BloomFilter(1000, 100);

			for (int i = 0 ; i < 100 ; i++) {
				v.Add(Guid.NewGuid().ToString());
				instance.add(v[i]);
			}

			Assert.IsTrue(instance.containsAll(v));
		}

		/**
		 * Test of getBit method, of class BloomFilter.
		 */
		[TestMethod]
		public void testGetBit()
		{
			Trace.WriteLine("getBit");
			BloomFilter instance = new BloomFilter(1000, 100);
			Random r = new Random();

			for (int i = 0 ; i < 100 ; i++) {
				bool b = r.Next(0, 2) == 1;
				instance.setBit(i, b);
				Assert.AreEqual(instance.getBit(i), b);
			}
		}

		/**
		 * Test of setBit method, of class BloomFilter.
		 */
		[TestMethod]
		public void testSetBit()
		{
			Trace.WriteLine("setBit");

			BloomFilter instance = new BloomFilter(1000, 100);
			Random r = new Random();

			for (int i = 0 ; i < 100 ; i++) {
				instance.setBit(i, true);
				Assert.AreEqual(instance.getBit(i), true);
			}

			for (int i = 0 ; i < 100 ; i++) {
				instance.setBit(i, false);
				Assert.AreEqual(instance.getBit(i), false);
			}
		}

		/**
		 * Test of size method, of class BloomFilter.
		 */
		[TestMethod]
		public void testSize()
		{
			Trace.WriteLine("size");
			for (int i = 100 ; i < 1000 ; i++) {
				BloomFilter instance = new BloomFilter(i, 10);
				Assert.AreEqual(instance.size(), i);
			}
		}

		/** Test error rate *
		 * @
		 */
		[TestMethod]
		public void testFalsePositiveRate1()
		{
			// Numbers are from // http://pages.cs.wisc.edu/~cao/papers/summary-cache/node8.html
			Trace.WriteLine("falsePositiveRate1");

			for (int j = 10 ; j < 21 ; j++) {
				Trace.Write(j-9 + "/11");
				List<byte[]> v = new List<byte[]>();
				BloomFilter instance = new BloomFilter(100*j, 100);

				for (int i = 0 ; i < 100 ; i++) {
					byte[] bytes = new byte[100];
					r.NextBytes(bytes);
					v.Add(bytes);
				}
				instance.addAll(v);

				long f = 0;
				double tests = 10000;
				for (int i = 0 ; i < tests ; i++) {
					byte[] bytes = new byte[100];
					r.NextBytes(bytes);
					if (instance.contains(bytes)) {
						if (!v.Contains(bytes)) {
							f++;
						}
					}
				}

				double ratio = f / tests;

				Trace.WriteLine(" - got " + ratio + ", math says " + instance.expectedFalsePositiveProbability());
				Assert.AreEqual(instance.expectedFalsePositiveProbability(), ratio, 0.01);
			}
		}

		/** Test for correct k **/
		[TestMethod]
		public void testGetK()
		{
			// Numbers are from http://pages.cs.wisc.edu/~cao/papers/summary-cache/node8.html
			Trace.WriteLine("testGetK");
			BloomFilter instance = null;

			instance = new BloomFilter(2, 1);
			Assert.AreEqual(1, instance.getK());

			instance = new BloomFilter(3, 1);
			Assert.AreEqual(2, instance.getK());

			instance = new BloomFilter(4, 1);
			Assert.AreEqual(3, instance.getK());

			instance = new BloomFilter(5, 1);
			Assert.AreEqual(3, instance.getK());

			instance = new BloomFilter(6, 1);
			Assert.AreEqual(4, instance.getK());

			instance = new BloomFilter(7, 1);
			Assert.AreEqual(5, instance.getK());

			instance = new BloomFilter(8, 1);
			Assert.AreEqual(6, instance.getK());

			instance = new BloomFilter(9, 1);
			Assert.AreEqual(6, instance.getK());

			instance = new BloomFilter(10, 1);
			Assert.AreEqual(7, instance.getK());

			instance = new BloomFilter(11, 1);
			Assert.AreEqual(8, instance.getK());

			instance = new BloomFilter(12, 1);
			Assert.AreEqual(8, instance.getK());
		}

		/**
		 * Test of contains method, of class BloomFilter.
		 */
		[TestMethod]
		public void testContains_GenericType()
		{
			Trace.WriteLine("contains");
			int items = 100;
			BloomFilter instance = new BloomFilter(0.01, items);

			for (int i = 0 ; i < items ; i++) {
				string s = Guid.NewGuid().ToString();
				instance.add(s);
				Assert.IsTrue(instance.contains(s));
			}
		}

		/**
		 * Test of contains method, of class BloomFilter.
		 */
		[TestMethod]
		public void testContains_byteArr()
		{
			Trace.WriteLine("contains");

			int items = 100;
			BloomFilter instance = new BloomFilter(0.01, items);

			for (int i = 0 ; i < items ; i++) {
				byte[] bytes = new byte[500];
				r.NextBytes(bytes);
				instance.add(bytes);
				Assert.IsTrue(instance.contains(bytes));
			}
		}

		/**
		 * Test of count method, of class BloomFilter.
		 */
		[TestMethod]
		public void testCount()
		{
			Trace.WriteLine("count");
			int expResult = 100;
			BloomFilter instance = new BloomFilter(0.01, expResult);
			for (int i = 0 ; i < expResult ; i++) {
				byte[] bytes = new byte[100];
				r.NextBytes(bytes);
				instance.add(bytes);
			}
			int result = instance.count();
			Assert.AreEqual(expResult, result);

			instance = new BloomFilter(0.01, expResult);
			for (int i = 0 ; i < expResult ; i++) {
				instance.add(Guid.NewGuid().ToString());
			}
			result = instance.count();
			Assert.AreEqual(expResult, result);
		}
	}
}
