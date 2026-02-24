<?php
/*
  This code is part of FusionDirectory (http://www.fusiondirectory.org/)
  Copyright (C) 2016-2023  FusionDirectory


  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

/* instanciation of CompositeAttribute in order to add check value features */
class ssoOptionsCharSeparatedCompositeAttribute extends CompositeAttribute
{
  function __construct ($label, $description, $ldapName, $acl = "")
  {


   parent::__construct(
      $description, $ldapName,
      [
        new SelectAttribute(
          _('options'), _('options'),
          'fdAccessApplicationSettings_var', TRUE,
          array(), '',
          array()
        ),
        new StringAttribute(
          _('Value'), _('value'),
          'fdAccessApplicationSettings_value', TRUE
        )
      ],
      '/^(.*)\|(.*)$/',
      '%s|%s',
      $acl, $label
    );
  }

 
  function check ()
  {
     $rule=$this->getParent()->getRemoteSSOMapping()[$this->attributes[0]->getValue()];
     if ( !$rule['range']){
       $resultTest = accessRemoteValidation::runTest($rule['validation'],$this->attributes[1]->getValue());
       $resultMessage= $rule['syntax'];
     }else{
       $resultTest  = accessRemoteValidation::runTest($rule['validation'],$this->attributes[1]->getValue(),$rule['range']);
       $resultMessage= $rule['syntax']." with value ".implode(",",$rule['range']);
     }

     if ( ! $resultTest ){
       return new SimplePluginCheckError(
         $this,
         htmlescape(sprintf("Error on %s attribute : value provided %s. Please see : %s",$this->attributes[0]->getValue(),$this->attributes[1]->getValue(),$resultMessage))
       );
       $this->attributes[1]->setValue($this->attributes[0]->getValue());
     }else{
       $this->attributes[1]->setValue($this->attributes[1]->getValue());
     }
  }
  
}


class oidcCompositeAttributes extends CharSeparatedCompositeAttribute
 {
  function __construct ( $label, $description, $ldapName, $acl = ""){

    $attributes=array(
      new ObjectSelectAttribute(
        _('Attribut'), _('Attribut'),
        'fdAccessApplicationAttributes_dn', TRUE,['attributes']
        ),
  
      new SelectAttribute(
        _('Value type'), _('type of value'),
        'fdAccessApplicationAttributes_type', TRUE, ['bool','int','string'],'string',['boolean','integer','string']
        ),
      new SelectAttribute(
        _('Table Distribution'), _('Distribute as table'),
        'fdAccessApplicationAttributes_table', TRUE, ['auto','always','never'],'auto',['automatic','always','never']
        )
    );
    parent::__construct($description, $ldapName, $attributes,'|','','');  

  }
}

class oidcScopeCompositeAttributes extends CharSeparatedCompositeAttribute
{
 function __construct ( $label, $description, $ldapName, $acl = ""){

   $attributes=array(
    new StringAttribute(
      _('Scope name'), _('OIDC scope'),
      'fdOIDCScope_name', FALSE, ""
      ), 
    new StringAttribute(
       _('Attributs'), _('Attributs'),
       'fdOIDCScope_dn', FALSE,""
       ),
     new StringAttribute(
       _('OIDC rules'), _('OIDC scope rules'),
       'fdOIDCScope_rule', FALSE, ""
       ), 
   );
   parent::__construct($description, $ldapName, $attributes,'|','','');  

 }
}

class samlCompositeAttributes extends CharSeparatedCompositeAttribute
{
  function __construct ( $label, $description, $ldapName, $acl = ""){
    $attributes=array(
      new ObjectSelectAttribute(
        _('Attribut'), _('Attribut'),
        'fdAccessApplicationAttributes_dn', FALSE,['attributes']
        ),
      new BooleanAttribute(
        _('Mandatory'), _('options'),
        'fdAccessApplicationAttributes_mandatory', FALSE, FALSE
        ),
      new SelectAttribute(
        _('SAML Format'), _('Format to distribute attribute'),
        'fdAccessApplicationAttributes_samlFormat', FALSE, ['urn:oasis:names:tc:SAML:2.0:attrname-format:uri','urn:oasis:names:tc:SAML:2.0:attrname-format:basic','urn:oasis:names:tc:SAML:2.0:attrname-format:unspecified'],'',['URI','Basic','unspecified']
      )
      );
      parent::__construct($description, $ldapName, $attributes,'|','',''); 
    }
}

class casCompositeAttributes extends CharSeparatedCompositeAttribute
{
  function __construct ( $label, $description, $ldapName, $acl = ""){
    $attributes=array(
      new ObjectSelectAttribute(
        _('Attribut'), _('Attribut'),
        'fdAccessApplicationAttributes_dn', FALSE,['attributes']
        ),
      new StringAttribute(
        _('Remote name'), _('Remote Name'),
        'fdAccessApplicationAttributes_name', FALSE, ""
        )
    );
      parent::__construct($description, $ldapName, $attributes,'|','',''); 
    }
}  
