<?php

/*
 * This file is part of the Symfony package.
 *
 * (c) Fabien Potencier <fabien@symfony.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

namespace Symfony\Component\Serializer\Normalizer;

use Symfony\Component\Serializer\Exception\InvalidArgumentException;
use Symfony\Component\Serializer\Exception\NotNormalizableValueException;

/**
 * Normalizes an object implementing the {@see \DateTimeInterface} to a date string.
 * Denormalizes a date string to an instance of {@see \DateTime} or {@see \DateTimeImmutable}.
 *
 * @author Kévin Dunglas <dunglas@gmail.com>
 */
class DateTimeNormalizer implements NormalizerInterface, DenormalizerInterface
{
    const FORMAT_KEY = 'datetime_format';
    const TIMEZONE_KEY = 'datetime_timezone';

    private $format;
    private $timezone;

    private static $supportedTypes = array(
        \DateTimeInterface::class => true,
        \DateTimeImmutable::class => true,
        \DateTime::class => true,
    );

    /**
     * @param string             $format
     * @param \DateTimeZone|null $timezone
     */
    public function __construct($format = \DateTime::RFC3339, \DateTimeZone $timezone = null)
    {
        $this->format = $format;
        $this->timezone = $timezone;
    }

    /**
     * {@inheritdoc}
     *
     * @throws InvalidArgumentException
     */
    public function normalize($object, $format = null, array $context = array())
    {
        if (!$object instanceof \DateTimeInterface) {
            throw new InvalidArgumentException('The object must implement the "\DateTimeInterface".');
        }

        $format = isset($context[self::FORMAT_KEY]) ? $context[self::FORMAT_KEY] : $this->format;
        $timezone = $this->getTimezone($context);

        if (null !== $timezone) {
            $object = (new \DateTimeImmutable('@'.$object->getTimestamp()))->setTimezone($timezone);
        }

        return $object->format($format);
    }

    /**
     * {@inheritdoc}
     */
    public function supportsNormalization($data, $format = null)
    {
        return $data instanceof \DateTimeInterface;
    }

    /**
     * {@inheritdoc}
     *
     * @throws NotNormalizableValueException
     */
    public function denormalize($data, $class, $format = null, array $context = array())
    {
        $dateTimeFormat = isset($context[self::FORMAT_KEY]) ? $context[self::FORMAT_KEY] : null;
        $timezone = $this->getTimezone($context);

        if (null !== $dateTimeFormat) {
            if (null === $timezone && PHP_VERSION_ID < 70000) {
                // https://bugs.php.net/bug.php?id=68669
                $object = \DateTime::class === $class ? \DateTime::createFromFormat($dateTimeFormat, $data) : \DateTimeImmutable::createFromFormat($dateTimeFormat, $data);
            } else {
                $object = \DateTime::class === $class ? \DateTime::createFromFormat($dateTimeFormat, $data, $timezone) : \DateTimeImmutable::createFromFormat($dateTimeFormat, $data, $timezone);
            }

            if (false !== $object) {
                return $object;
            }

            $dateTimeErrors = \DateTime::class === $class ? \DateTime::getLastErrors() : \DateTimeImmutable::getLastErrors();

            throw new NotNormalizableValueException(sprintf(
                'Parsing datetime string "%s" using format "%s" resulted in %d errors:'."\n".'%s',
                $data,
                $dateTimeFormat,
                $dateTimeErrors['error_count'],
                implode("\n", $this->formatDateTimeErrors($dateTimeErrors['errors']))
            ));
        }

        try {
            return \DateTime::class === $class ? new \DateTime($data, $timezone) : new \DateTimeImmutable($data, $timezone);
        } catch (\Exception $e) {
            throw new NotNormalizableValueException($e->getMessage(), $e->getCode(), $e);
        }
    }

    /**
     * {@inheritdoc}
     */
    public function supportsDenormalization($data, $type, $format = null)
    {
        return isset(self::$supportedTypes[$type]);
    }

    /**
     * Formats datetime errors.
     *
     * @return string[]
     */
    private function formatDateTimeErrors(array $errors)
    {
        $formattedErrors = array();

        foreach ($errors as $pos => $message) {
            $formattedErrors[] = sprintf('at position %d: %s', $pos, $message);
        }

        return $formattedErrors;
    }

    private function getTimezone(array $context)
    {
        $dateTimeZone = array_key_exists(self::TIMEZONE_KEY, $context) ? $context[self::TIMEZONE_KEY] : $this->timezone;

        if (null === $dateTimeZone) {
            return null;
        }

        return $dateTimeZone instanceof \DateTimeZone ? $dateTimeZone : new \DateTimeZone($dateTimeZone);
    }
}
