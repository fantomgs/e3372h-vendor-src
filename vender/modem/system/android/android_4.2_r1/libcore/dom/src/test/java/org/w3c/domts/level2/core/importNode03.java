
/*
This Java source file was generated by test-to-java.xsl
and is a derived work from the source document.
The source document contained the following notice:



Copyright (c) 2001-2004 World Wide Web Consortium, 
(Massachusetts Institute of Technology, Institut National de
Recherche en Informatique et en Automatique, Keio University).  All 
Rights Reserved.  This program is distributed under the W3C's Software
Intellectual Property License.  This program is distributed in the 
hope that it will be useful, but WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
PURPOSE.  

See W3C License http://www.w3.org/Consortium/Legal/ for more details.


*/

package org.w3c.domts.level2.core;

import org.w3c.dom.*;


import org.w3c.domts.DOMTestCase;
import org.w3c.domts.DOMTestDocumentBuilderFactory;



/**
 *     The "importNode(importedNode,deep)" method for a 
 *    Document should import the given importedNode into that Document.
 *    The importedNode is of type Comment.
 *    
 *    Create a comment node with value being the string "this is a comment" in 
 *    a different document.  Invoke method importNode(importedNode,deep) on
 *    this document.  Method should return a comment node whose value matches
 *    the above string. The returned node should belong to this document whose
 *    systemId is "staff.dtd"
* @author NIST
* @author Mary Brady
* @see <a href="http://www.w3.org/TR/DOM-Level-2-Core/core#Core-Document-importNode">http://www.w3.org/TR/DOM-Level-2-Core/core#Core-Document-importNode</a>
*/
public final class importNode03 extends DOMTestCase {

   /**
    * Constructor.
    * @param factory document factory, may not be null
    * @throws org.w3c.domts.DOMTestIncompatibleException Thrown if test is not compatible with parser configuration
    */
   public importNode03(final DOMTestDocumentBuilderFactory factory)  throws org.w3c.domts.DOMTestIncompatibleException {
      super(factory);

    //
    //   check if loaded documents are supported for content type
    //
    String contentType = getContentType();
    preload(contentType, "staffNS", true);
    preload(contentType, "staffNS", true);
    }

   /**
    * Runs the test case.
    * @throws Throwable Any uncaught exception causes test to fail
    */
   public void runTest() throws Throwable {
      Document doc;
      Document aNewDoc;
      Comment comment;
      Node aNode;
      Document ownerDocument;
      DocumentType docType;
      String system;
      String value;
      doc = (Document) load("staffNS", true);
      aNewDoc = (Document) load("staffNS", true);
      comment = aNewDoc.createComment("this is a comment");
      aNode = doc.importNode(comment, false);
      ownerDocument = aNode.getOwnerDocument();
      assertNotNull("ownerDocumentNotNull", ownerDocument);
      docType = ownerDocument.getDoctype();
      system = docType.getSystemId();
      assertURIEquals("systemId", null, null, null, "staffNS.dtd", null, null, null, null, system);
value = aNode.getNodeValue();
      assertEquals("nodeValue", "this is a comment", value);
      }
   /**
    *  Gets URI that identifies the test.
    *  @return uri identifier of test
    */
   public String getTargetURI() {
      return "http://www.w3.org/2001/DOM-Test-Suite/level2/core/importNode03";
   }
   /**
    * Runs this test from the command line.
    * @param args command line arguments
    */
   public static void main(final String[] args) {
        DOMTestCase.doMain(importNode03.class, args);
   }
}

