<?php

/*
 * This file is part of the Symfony package.
 *
 * (c) Fabien Potencier <fabien@symfony.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

namespace Symfony\Component\Serializer\Encoder;

use Symfony\Component\Serializer\SerializerAwareInterface;
use Symfony\Component\Serializer\SerializerAwareTrait;

/**
 * SerializerAware Encoder implementation.
 *
 * @author Jordi Boggiano <j.boggiano@seld.be>
 *
 * @deprecated since version 3.2, to be removed in 4.0. Use the SerializerAwareTrait instead.
 */
abstract class SerializerAwareEncoder implements SerializerAwareInterface
{
    use SerializerAwareTrait;
}
