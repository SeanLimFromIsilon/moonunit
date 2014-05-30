<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet
   version="1.0"
   xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
   xmlns="http://www.w3.org/1999/xhtml">
  <xsl:output 
      method="xml"
      indent="yes"
      encoding="UTF-8"
      doctype-public="-//W3C//DTD XHTML 1.0 Strict//EN"
      doctype-system="http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd"/>
  <xsl:strip-space elements="*"/>

  <!-- Emit main page -->
  <xsl:template match="/moonunit">
    <html>
      <head>
	<xsl:choose>
	  <xsl:when test="@title">
	    <title>MoonUnit: <xsl:value-of select="@title"/></title>
	  </xsl:when>
	  <xsl:otherwise>
	    <title>MoonUnit: Test Results</title>
	  </xsl:otherwise>
	</xsl:choose>
	<link rel="stylesheet" type="text/css" href="muxhtml.css"/>
	<script type="text/javascript" src="jquery-1.6.2.min.js">
	  <xsl:comment>Nothing to see here</xsl:comment>
	</script>
	<script type="text/javascript" src="muxhtml.js">
	  <xsl:comment>This space intentionally left blank</xsl:comment>
	</script>
      </head>
      <body>
	<div class="main">
	  <div class="header">
	    <xsl:if test="@name">
	      <span class="title">
		<xsl:value-of select="@name"/>
	      </span>
	    </xsl:if>
	  </div>
	  <div class="content">
	    <xsl:call-template name="emit-summary"/>
	    <xsl:call-template name="emit-controls"/>
	    <xsl:for-each select="run">
	      <xsl:call-template name="emit-run"/>
	    </xsl:for-each>
	  </div>
	</div>
      </body>
    </html>
  </xsl:template>

  <xsl:template name="emit-summary">
    <div class="summary">
      <div class="header">
	<span class="title">Summary</span>
      </div>
      <div class="content">
	<div class="statistic" name="tests">
	  <div>Tests</div>
	  <div><xsl:value-of select="count(.//test)"/></div>
	</div>
	<div class="statistic" name="passed">
	  <div>Passed</div>
	  <div>
	    <xsl:value-of 
	       select="count(.//test[result/@status = 'pass' or result/@status = 'xfail'])"
	       />
	  </div>	 
	</div>
	<div class="statistic" name="failed">
	  <div>Failed</div>
	  <div>
	    <xsl:value-of 
	       select="count(.//test[result/@status = 'fail' or result/@status = 'xpass'])"
	       />
	  </div>
	</div>
	<div class="statistic" name="skipped">
	  <div>Skipped</div>
	  <div>
	    <xsl:value-of 
	       select="count(.//test[result/@status = 'skip'])"
	       />
	  </div>
	</div>
      </div>
    </div>
  </xsl:template>

  <!-- Emit run entry -->
  <xsl:template name="emit-run">
    <div class="run">
      <div class="header">
	<span class="title">
	  <xsl:if test="@name">
	    <span name="name"><xsl:value-of select="@name"/></span>    
	  </xsl:if>
	  <span name="platform">
	    <xsl:value-of select="@cpu"/>
	    <xsl:text> </xsl:text>
	    <xsl:value-of select="@vendor"/>
	    <xsl:text> </xsl:text>
	    <xsl:value-of select="@os"/>
	  </span>
	</span>
      </div>
      <div class="content">
	<xsl:for-each select="library">
	  <xsl:call-template name="emit-library"/>
	</xsl:for-each>
      </div>
    </div>
  </xsl:template>

  <!-- Emit library entry -->
  <xsl:template name="emit-library">
    <div class="library">
      <xsl:attribute name="library-name">
	<xsl:value-of select="@name"/>
      </xsl:attribute>
      <div class="header">
	<div class="title"><xsl:value-of select="@name"/></div>
      </div>
      <div class="content">
	<xsl:for-each select="suite">
	  <xsl:call-template name="emit-suite"/>
	</xsl:for-each>
      </div>
    </div>
  </xsl:template>

  <!-- Emit suite entry -->
  <xsl:template name="emit-suite">
    <div class="suite">
      <xsl:attribute name="suite-name">
	<xsl:value-of select="@name"/>
      </xsl:attribute>
      <div class="header">
	<span class="title"><xsl:value-of select="@name"/></span>
      </div>
      <div class="content">
	<xsl:for-each select="test">
	  <xsl:call-template name="emit-test"/>
	</xsl:for-each>
      </div>
    </div>
  </xsl:template>

  <!-- Emit test entry -->
  <xsl:template name="emit-test">
    <xsl:variable name="status" select="result/@status"/>
    <div class="test">
      <xsl:attribute name="test-name">
	<xsl:value-of select="@name"/>
      </xsl:attribute>
      <xsl:attribute name="test-status">
	<xsl:value-of select="$status"/>
      </xsl:attribute>
      <div class="header">
	<xsl:if test="event or normalize-space(result/.)">
	  <xsl:attribute name="full">
	    <xsl:text>true</xsl:text>
	  </xsl:attribute>
	</xsl:if>
	<span class="result">
	  <xsl:attribute name="status">
	    <xsl:choose>
	      <xsl:when test="$status = 'pass' or $status = 'xfail'">
		<xsl:text>pass</xsl:text>
	      </xsl:when>
	      <xsl:when test="$status = 'fail' or $status = 'xpass'">
		<xsl:text>fail</xsl:text>
	      </xsl:when>
	      <xsl:when test="$status = 'skip'">
		<xsl:text>skip</xsl:text>
	      </xsl:when>
	    </xsl:choose>
	  </xsl:attribute>
	  <xsl:value-of select="$status"/>
	</span>
	<span class="title"><xsl:value-of select="@name"/></span>
      </div>
      <xsl:if test="event or normalize-space(result/.)">
	<div class="content">
	  <xsl:for-each select="event">
	    <xsl:call-template name="emit-event"/>
	  </xsl:for-each>
	  <xsl:if test="normalize-space(result)">
	    <div class="result-message">
	      <div class="message-type">
		<span class="result-tag">result</span>
	      </div>
	      <!-- Emit file/line number if we know it -->
	    <xsl:if test="result/@file">
	      <div class="message-source">
		<span class="source-tag">
		    <xsl:value-of select="result/@file"/>
		    <xsl:text>:</xsl:text>
		    <xsl:value-of select="result/@line"/>
		</span>
	      </div>
	     </xsl:if>
	      <div class="message-content">
		<xsl:value-of select="result"/>
	      </div>
	    </div>
	  </xsl:if>
	</div>
      </xsl:if>
    </div>
  </xsl:template>
  
  <!-- Emit event entry -->
  <xsl:template name="emit-event">
    <div class="event">
      <div class="message-type">
	<span class="level-tag">
	  <xsl:attribute name="level">
	    <xsl:value-of select="@level"/>
	  </xsl:attribute>
	  <xsl:value-of select="@level"/>
	</span>
      </div>
      <div class="message-source">
	<span class="source-tag">
	  <xsl:if test="@file">
	    <xsl:value-of select="@file"/>
	    <xsl:text>:</xsl:text>
	    <xsl:value-of select="@line"/>
	  </xsl:if>
	</span>
      </div>
      <div class="message-content">
	  <xsl:value-of select="."/>
      </div>
    </div>
  </xsl:template>

  <!-- Emit backtrace -->
  <xsl:template name="emit-backtrace">
  </xsl:template>
  
  <!-- Emit UI controls -->
  <xsl:template name="emit-controls">
    <div id="controls">
      <div class="header">Controls</div>
      <div class="content">
	<div id="filter-control">
	  <div class="header">Filter</div>
	  <div class="content">
	    <div class="inputs">
	      <div class="input">
		<label>Search</label><input type="text" id="filter-name"/>
	      </div>
	      <div class="input">
		<input type="checkbox" checked="true" id="filter-pass"/><label>Pass</label>
		<input type="checkbox" checked="true" id="filter-fail"/><label>Fail</label>
		<input type="checkbox" checked="true" id="filter-skip"/><label>Skip</label>
	      </div>
	    </div>
	    <div class="buttons">
	      <input type="button" value="Reset" id="filter-reset-button"/>
	    </div>
	  </div>
	</div>
      </div>
    </div>
  </xsl:template>
</xsl:stylesheet>
